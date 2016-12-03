// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "GraphEditorActions.h"
#include "ScopedTransaction.h"
#include "EdGraphUtilities.h"

#include "AI/SelectionQuery/SelectionQuery.h"
//#include "AI/SelectionQuery/SQCompositeNode.h"

#include "Toolkits/IToolkitHost.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Editor/PropertyEditor/Public/IDetailsView.h"
#include "SDockTab.h"
#include "GenericCommands.h"
 
#define LOCTEXT_NAMESPACE "SelectionQueryEditor"

const FName FSelectionQueryEditorTabs::UpdateGraphId( TEXT( "SelectionQueryEditor_UpdateGraph" ) );
const FName FSelectionQueryEditorTabs::PropertiesId( TEXT( "SelectionQueryEditor_Properties" ) );

// Begin IToolkit interface
void FSelectionQueryEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_SelectionQueryEditor", "Selection Query Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FSelectionQueryEditorTabs::UpdateGraphId, FOnSpawnTab::CreateSP(this, &FSelectionQueryEditor::SpawnTab_UpdateGraph) )
		.SetDisplayName( NSLOCTEXT("SelectionQueryEditor", "Graph", "Graph") )
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FSelectionQueryEditorTabs::PropertiesId, FOnSpawnTab::CreateSP(this, &FSelectionQueryEditor::SpawnTab_Properties) )
		.SetDisplayName( NSLOCTEXT("SelectionQueryEditor", "PropertiesTab", "Details" ) )
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}
void FSelectionQueryEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FSelectionQueryEditorTabs::PropertiesId);
	InTabManager->UnregisterTabSpawner(FSelectionQueryEditorTabs::UpdateGraphId);
}
// End IToolkit interface

// Begin FAssetEditorToolkit interface
FName FSelectionQueryEditor::GetToolkitFName() const
{
	return FName("Selection Query");
}
FText FSelectionQueryEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "SelectionQuery");
}
FText FSelectionQueryEditor::GetToolkitName() const
{
    return LOCTEXT("AppLabel", "SelectionQuery");
}
FLinearColor FSelectionQueryEditor::GetWorldCentricTabColorScale() const
{
    return FLinearColor(0.0f, 0.0f, 0.2f, 0.5f);
}
FString FSelectionQueryEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "SelectionQuery ").ToString();
}
// End FAssetEditorToolkit interface

// Begin FTickableGameObject Interface
void FSelectionQueryEditor::Tick(float DeltaTime)
{
    if (bGraphStateChanged) {
        bGraphStateChanged = false;
        HandleGraphChanged();
    }
}
// End FTickableGameObject Interface

void FSelectionQueryEditor::InitSelectionQueryEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, USelectionQuery* InScript)
{
    //SelectedNodesCount = 0;
    Query = InScript;
    check(Query != NULL);

    TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_SelectionQuery_Layout")
        ->AddArea
        (
            FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
            ->Split
            (
                FTabManager::NewStack()
                ->SetSizeCoefficient(0.1f)
                ->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
                ->SetHideTabWell(true)
            )
            ->Split
            (
                FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.7f)
                    ->AddTab(FSelectionQueryEditorTabs::UpdateGraphId, ETabState::OpenedTab)
                )
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.3f)
                    ->AddTab(FSelectionQueryEditorTabs::PropertiesId, ETabState::OpenedTab)
                )
            )
        );

    const bool bCreateDefaultStandaloneMenu = true;
    const bool bCreateDefaultToolbar = true;
    FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, FSelectionQueryEditorModule::SelectionQueryEditorAppIdentifier, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, Query);

    FSelectionQueryEditorModule& SelectionQueryEditorModule = FModuleManager::LoadModuleChecked<FSelectionQueryEditorModule>("SelectionQueryEditor");
    AddMenuExtender(SelectionQueryEditorModule.GetMenuExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));

    RegenerateMenusAndToolbars();

    // Update SQ asset data based on saved graph to have correct data in editor
    if (GraphEditor.IsValid() && GraphEditor->GetCurrentGraph() != NULL)
    {
        //let's find root node
        //USelectionQueryGraph* SQGraph = Cast<USelectionQueryGraph>(GraphEditor->GetCurrentGraph());
        //SQGraph->UpdateAsset();
    }
}

void FSelectionQueryEditor::ShowObjectDetails(UObject* ObjectProperties)
{
    DetailsView->SetObject(ObjectProperties);
}

void FSelectionQueryEditor::SelectAllNodes()
{
    GraphEditor->SelectAllNodes();
}

bool FSelectionQueryEditor::CanSelectAllNodes() const
{
    return GraphEditor.IsValid();
}

void FSelectionQueryEditor::DeleteSelectedNodes()
{
    TArray<UEdGraphNode*> NodesToDelete;
    const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

    for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
    {
        NodesToDelete.Add(CastChecked<UEdGraphNode>(*NodeIt));
    }

    DeleteNodes(NodesToDelete);
}

bool FSelectionQueryEditor::CanDeleteNode(class UEdGraphNode* Node)
{
    bool CanDelete = true;
    /**
    if (UEdGraphNode_RootNode* MarkerNode = Cast<UEdGraphNode_RootNode>(Node)) {
        CanDelete = MarkerNode->bUserDefined;
    }*/

    return CanDelete;
}

void FSelectionQueryEditor::DeleteNodes(const TArray<class UEdGraphNode*>& NodesToDelete)
{
    if (NodesToDelete.Num() > 0)
    {

        for (int32 Index = 0; Index < NodesToDelete.Num(); ++Index)
        {
            if (!CanDeleteNode(NodesToDelete[Index])) {
                continue;
            }

            // Break all node links first so that we don't update the material before deleting
            NodesToDelete[Index]->BreakAllNodeLinks();

            FBlueprintEditorUtils::RemoveNode(NULL, NodesToDelete[Index], true);

            // TODO: Process deletion in the data model
        }
    }
}

void FSelectionQueryEditor::DeleteSelectedDuplicatableNodes()
{
    // Cache off the old selection
    const FGraphPanelSelectionSet OldSelectedNodes = GraphEditor->GetSelectedNodes();

    // Clear the selection and only select the nodes that can be duplicated
    FGraphPanelSelectionSet RemainingNodes;
    GraphEditor->ClearSelectionSet();

    for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
    {
        UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
        if ((Node != NULL) && Node->CanDuplicateNode())
        {
            GraphEditor->SetNodeSelection(Node, true);
        }
        else
        {
            RemainingNodes.Add(Node);
        }
    }

    // Delete the duplicatable nodes
    DeleteSelectedNodes();

    // Reselect whatever is left from the original selection after the deletion
    GraphEditor->ClearSelectionSet();

    for (FGraphPanelSelectionSet::TConstIterator SelectedIter(RemainingNodes); SelectedIter; ++SelectedIter)
    {
        if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
        {
            GraphEditor->SetNodeSelection(Node, true);
        }
    }
}

bool FSelectionQueryEditor::CanDeleteNodes() const
{
    return true;
}

void FSelectionQueryEditor::CopySelectedNodes()
{
    // Export the selected nodes and place the text on the clipboard
    const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

    FString ExportedText;

    for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
    {
        if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
        {
            Node->PrepareForCopying();
        }
    }

    FEdGraphUtilities::ExportNodesToText(SelectedNodes, /*out*/ ExportedText);
    FPlatformMisc::ClipboardCopy(*ExportedText);

    // Make sure the owner remains the same for the copied node
    // TODO: Check MaterialEditor.cpp for reference

}

bool FSelectionQueryEditor::CanCopyNodes() const
{
    // If any of the nodes can be duplicated then we should allow copying
    const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
    for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
    {
        UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
        if ((Node != NULL) && Node->CanDuplicateNode())
        {
            return true;
        }
    }
    return false;
}

void FSelectionQueryEditor::PasteNodes()
{

    PasteNodesHere(GraphEditor->GetPasteLocation());
}

void FSelectionQueryEditor::PasteNodesHere(const FVector2D& Location)
{
    // Undo/Redo support
    const FScopedTransaction Transaction(NSLOCTEXT("DungeonArchitect", "DungeonEditorPaste", "Dungeon Editor: Paste"));
    // TODO: Notify the data model of modification
    //Material->MaterialGraph->Modify();
    //Material->Modify();

    // Clear the selection set (newly pasted stuff will be selected)
    GraphEditor->ClearSelectionSet();

    // Grab the text to paste from the clipboard.
    FString TextToImport;
    FPlatformMisc::ClipboardPaste(TextToImport);

    // Import the nodes
    if (!Query) return;
    TSet<UEdGraphNode*> PastedNodes;
    FEdGraphUtilities::ImportNodesFromText(Query->EdGraph, TextToImport, /*out*/ PastedNodes);

    //Average position of nodes so we can move them while still maintaining relative distances to each other
    FVector2D AvgNodePosition(0.0f, 0.0f);

    for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
    {
        UEdGraphNode* Node = *It;
        AvgNodePosition.X += Node->NodePosX;
        AvgNodePosition.Y += Node->NodePosY;
    }

    if (PastedNodes.Num() > 0)
    {
        float InvNumNodes = 1.0f / float(PastedNodes.Num());
        AvgNodePosition.X *= InvNumNodes;
        AvgNodePosition.Y *= InvNumNodes;
    }

    for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
    {
        UEdGraphNode* Node = *It;
        /*if (UEdGraphNode_CompositeNode* MeshNode = Cast<UEdGraphNode_DungeonMesh>(Node))
        {
            // TODO: Handle
        }
        else if (UEdGraphNode_ItemNode* MarkerNode = Cast<UEdGraphNode_DungeonMarker>(Node))
        {
            // TODO: Handle
        }*/

        // Select the newly pasted stuff
        GraphEditor->SetNodeSelection(Node, true);

        Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
        Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

        Node->SnapToGrid(SNodePanel::GetSnapGridSize());

        // Give new node a different Guid from the old one
        Node->CreateNewGuid();
    }

    // TODO: Implement
    //UpdatePropAfterGraphChange();

    // Update UI
    GraphEditor->NotifyGraphChanged();
}

bool FSelectionQueryEditor::CanPasteNodes() const
{
    FString ClipboardContent;
    FPlatformMisc::ClipboardPaste(ClipboardContent);

    return FEdGraphUtilities::CanImportNodesFromText(Query->EdGraph, ClipboardContent);
}

void FSelectionQueryEditor::CutSelectedNodes()
{
    CopySelectedNodes();
    // Cut should only delete nodes that can be duplicated
    DeleteSelectedDuplicatableNodes();
}

bool FSelectionQueryEditor::CanCutNodes() const
{
    return CanCopyNodes() && CanDeleteNodes();
}

void FSelectionQueryEditor::DuplicateNodes()
{
    // Copy and paste current selection
    CopySelectedNodes();
    PasteNodes();
}

bool FSelectionQueryEditor::CanDuplicateNodes() const
{
    return CanCopyNodes();
}

/** Create new tab for the supplied graph - don't call this directly, call SExplorer->FindTabForGraph.*/
TSharedRef<SGraphEditor> FSelectionQueryEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	check(InGraph != NULL);
	
	// Create the appearance info
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText", "SELECTION QUERY");

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FSelectionQueryEditor::OnSelectedNodesChanged);
	
	// Make title bar
	TSharedRef<SWidget> TitleBarWidget = 
		SNew(SBorder)
		.BorderImage( FEditorStyle::GetBrush( TEXT("Graph.TitleBackground") ) )
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("SelectionQueryEditor", "TheQueryGraphLabel", "Query Graph"))
				.TextStyle( FEditorStyle::Get(), TEXT("GraphBreadcrumbButtonText") )
			]
		];

	// Make full graph editor
	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.Appearance(AppearanceInfo)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph)
		.GraphEvents(InEvents);
}


TSharedRef<SDockTab> FSelectionQueryEditor::SpawnTab_UpdateGraph( const FSpawnTabArgs& Args )
{
	check( Args.GetTabId().TabType == FSelectionQueryEditorTabs::UpdateGraphId);

	/*USelectionQueryGraph* MyGraph = Cast<USelectionQueryGraph>(Query->EdGraph);
	if (Query->EdGraph == NULL)
	{
		MyGraph = NewObject<USelectionQueryGraph>(Query, NAME_None, RF_Transactional);
		Query->EdGraph = MyGraph;

		// let's read data from BT script and generate nodes
		const UEdGraphSchema* Schema = Query->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*Query->EdGraph);

		MyGraph->OnCreated();
	}
	else
	{
		MyGraph->OnLoaded();
	}

	MyGraph->Initialize();*/

	//TSharedRef<SGraphEditor> UpdateGraphEditor = CreateGraphEditorWidget(Query->EdGraph);
	//UpdateGraphEdPtr = UpdateGraphEditor; // Keep pointer to editor
    
    /*return SNew(SDockTab)
		.Label( NSLOCTEXT("SelectionQueryEditor", "UpdateGraph", "Update Graph") )
		.TabColorScale( GetTabColorScale() )
		[
			UpdateGraphEditor
		];*/
}

TSharedRef<SDockTab> FSelectionQueryEditor::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check( Args.GetTabId() == FSelectionQueryEditorTabs::PropertiesId);

	//CreateInternalWidgets();

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Icon( FEditorStyle::GetBrush("SoundClassEditor.Tabs.Properties") )
		.Label( LOCTEXT("SoundClassPropertiesTitle", "Details" ) )
		[
            DetailsView;
		];

	return SpawnedTab;
}

void FSelectionQueryEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	SelectedNodesCount = NewSelection.Num();
	if (NewSelection.Num())
	{
		for(TSet<class UObject*>::TConstIterator SetIt(NewSelection);SetIt;++SetIt)
		{
            /*USelectionQueryGraphNode* GraphNode = Cast<USelectionQueryGraphNode>(*SetIt);
			if (GraphNode)
			{
				if (GraphNode->IsA(USelectionQueryGraphNode_Root::StaticClass()))
				{
					Selection.Add(GraphNode);
				}
				else if (GraphNode->IsA(USelectionQueryGraphNode_Composite::StaticClass()))
				{
					USQCompositeNode* QueryComposite = Cast<USQCompositeNode>(GraphNode->NodeInstance);
					if (QueryComposite)
					{
						Selection.Add(QueryComposite);
					}
				}
				else
				{
					Selection.Add(GraphNode);
				}
			}*/
		}
	}

	if (Selection.Num() == 1)
	{
		DetailsView->SetObjects(Selection);
	}
	else
	{
		DetailsView->SetObject(NULL);
	}
}

void FSelectionQueryEditor::CreateInternalWidgets()
{
	FPropertyEditorModule& DetailsViewModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
	const FDetailsViewArgs DetailsViewArgs( false, false, true, FDetailsViewArgs::ObjectsUseNameArea, false );
	DetailsView = DetailsViewModule.CreateDetailView( DetailsViewArgs );
	DetailsView->SetObject( NULL );
	DetailsView->OnFinishedChangingProperties().AddSP(this, &FSelectionQueryEditor::OnFinishedChangingProperties);
}

void FSelectionQueryEditor::SaveAsset_Execute()
{
	// modify BT asset
	TSharedPtr<SGraphEditor> UpdateGraphEditor = UpdateGraphEdPtr.Pin();
	if (UpdateGraphEditor.IsValid() && UpdateGraphEditor->GetCurrentGraph() != NULL)
	{
		//let's find root node
		USelectionQueryGraph* EdGraph = Cast<USelectionQueryGraph>(UpdateGraphEditor->GetCurrentGraph());
		EdGraph->UpdateAsset();
	}
	// save it
	ISelectionQueryEditor::SaveAsset_Execute();
}

void FSelectionQueryEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property)
	{
		FGraphPanelSelectionSet CurrentSelection = GraphEditor->GetSelectedNodes();
		if (CurrentSelection.Num() == 1)
		{
			for (FGraphPanelSelectionSet::TConstIterator It(CurrentSelection); It; ++It)
			{
				//USelectionQueryGraphNode_Item* ItemNode = Cast<USelectionQueryGraphNode_Item>(*It);
				/*USelectionQueryGraphNode_Composite* ParentNode = ItemNode ? Cast<USelectionQueryGraphNode_Composite>(ItemNode->ParentNode) : nullptr;
                
				if (ParentNode)
				{
					//ParentNode->CalculateWeights();
					break;
				}*/
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE