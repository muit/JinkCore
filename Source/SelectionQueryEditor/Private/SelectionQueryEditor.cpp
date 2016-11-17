// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "GraphEditorActions.h"
#include "ScopedTransaction.h"
#include "EdGraphUtilities.h"

#include "AI/SelectionQuery/SelectionQuery.h"
#include "AI/SelectionQuery/SQCompositeNode.h"

#include "Toolkits/IToolkitHost.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Editor/PropertyEditor/Public/IDetailsView.h"
#include "SDockTab.h"
#include "GenericCommands.h"
 
#define LOCTEXT_NAMESPACE "SelectionQueryEditor"

const FName FSelectionQueryEditor::SQUpdateGraphTabId( TEXT( "SelectionQueryEditor_UpdateGraph" ) );
const FName FSelectionQueryEditor::SQPropertiesTabId( TEXT( "SelectionQueryEditor_Properties" ) );

void FSelectionQueryEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_SelectionQueryEditor", "Selection Query Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner( SQUpdateGraphTabId, FOnSpawnTab::CreateSP(this, &FSelectionQueryEditor::SpawnTab_UpdateGraph) )
		.SetDisplayName( NSLOCTEXT("SelectionQueryEditor", "Graph", "Graph") )
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner( SQPropertiesTabId, FOnSpawnTab::CreateSP(this, &FSelectionQueryEditor::SpawnTab_Properties) )
		.SetDisplayName( NSLOCTEXT("SelectionQueryEditor", "PropertiesTab", "Details" ) )
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FSelectionQueryEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner( SQPropertiesTabId );
	InTabManager->UnregisterTabSpawner( SQUpdateGraphTabId );
}

void FSelectionQueryEditor::InitSelectionQueryEditor( const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, USelectionQuery* InScript )
{
	SelectedNodesCount = 0;
	Query = InScript;
	check(Query != NULL);

	TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout( "Standalone_SelectionQuery_Layout" )
	->AddArea
	(
		FTabManager::NewPrimaryArea() ->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewStack()
			->SetSizeCoefficient(0.1f)
			->AddTab(GetToolbarTabId(), ETabState::OpenedTab) 
			->SetHideTabWell( true )
		)
		->Split
		(
			FTabManager::NewSplitter() ->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.7f)
				->AddTab( SQUpdateGraphTabId, ETabState::OpenedTab )
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.3f)
				->AddTab( SQPropertiesTabId, ETabState::OpenedTab )
			)
		)
	);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor( Mode, InitToolkitHost, FSelectionQueryEditorModule::SelectionQueryEditorAppIdentifier, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, Query );
	
	FSelectionQueryEditorModule& SelectionQueryEditorModule = FModuleManager::LoadModuleChecked<FSelectionQueryEditorModule>( "SelectionQueryEditor" );
	AddMenuExtender(SelectionQueryEditorModule.GetMenuExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));

	RegenerateMenusAndToolbars();

	// Update BT asset data based on saved graph to have correct data in editor
	TSharedPtr<SGraphEditor> UpdateGraphEditor = UpdateGraphEdPtr.Pin();
	if (UpdateGraphEditor.IsValid() && UpdateGraphEditor->GetCurrentGraph() != NULL)
	{
		//let's find root node
		USelectionQueryGraph* SQGraph = Cast<USelectionQueryGraph>(UpdateGraphEditor->GetCurrentGraph());
		SQGraph->UpdateAsset();
	}
}

FName FSelectionQueryEditor::GetToolkitFName() const
{
	return FName("Selection Query");
}

FText FSelectionQueryEditor::GetBaseToolkitName() const
{
	return NSLOCTEXT("SelectionQueryEditor", "AppLabel", "SelectionQuery");
}

FString FSelectionQueryEditor::GetWorldCentricTabPrefix() const
{
	return NSLOCTEXT("SelectionQueryEditor", "WorldCentricTabPrefix", "SelectionQuery ").ToString();
}


FLinearColor FSelectionQueryEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor( 0.0f, 0.0f, 0.2f, 0.5f );
}


/** Create new tab for the supplied graph - don't call this directly, call SExplorer->FindTabForGraph.*/
TSharedRef<SGraphEditor> FSelectionQueryEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	check(InGraph != NULL);
	
	// Create the appearance info
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = NSLOCTEXT("SelectionQueryEditor", "AppearanceCornerText", "SELECTION QUERY");

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FSelectionQueryEditor::OnSelectedNodesChanged);
	
	CreateCommandList();

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
	check( Args.GetTabId().TabType == SQUpdateGraphTabId );
	USelectionQueryGraph* MyGraph = Cast<USelectionQueryGraph>(Query->EdGraph);
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

	MyGraph->Initialize();

	TSharedRef<SGraphEditor> UpdateGraphEditor = CreateGraphEditorWidget(Query->EdGraph);
	UpdateGraphEdPtr = UpdateGraphEditor; // Keep pointer to editor

	return SNew(SDockTab)
		.Label( NSLOCTEXT("SelectionQueryEditor", "UpdateGraph", "Update Graph") )
		.TabColorScale( GetTabColorScale() )
		[
			UpdateGraphEditor
		];
}

TSharedRef<SDockTab> FSelectionQueryEditor::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check( Args.GetTabId() == SQPropertiesTabId );

	CreateInternalWidgets();

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Icon( FEditorStyle::GetBrush("SoundClassEditor.Tabs.Properties") )
		.Label( NSLOCTEXT("SelectionQueryEditor", "SoundClassPropertiesTitle", "Details" ) )
		[
			DetailsView.ToSharedRef()
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
			USelectionQueryGraphNode* GraphNode = Cast<USelectionQueryGraphNode>(*SetIt);
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
					Selection.Add(GraphNode->NodeInstance);
				}
			}
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
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
	const FDetailsViewArgs DetailsViewArgs( false, false, true, FDetailsViewArgs::ObjectsUseNameArea, false );
	DetailsView = PropertyEditorModule.CreateDetailView( DetailsViewArgs );
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
		FGraphPanelSelectionSet CurrentSelection = GetSelectedNodes();
		if (CurrentSelection.Num() == 1)
		{
			for (FGraphPanelSelectionSet::TConstIterator It(CurrentSelection); It; ++It)
			{
				USelectionQueryGraphNode_Item* ItemNode = Cast<USelectionQueryGraphNode_Item>(*It);
				USelectionQueryGraphNode_Composite* ParentNode = ItemNode ? Cast<USelectionQueryGraphNode_Composite>(ItemNode->ParentNode) : nullptr;

				if (ParentNode)
				{
					//ParentNode->CalculateWeights();
					break;
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE