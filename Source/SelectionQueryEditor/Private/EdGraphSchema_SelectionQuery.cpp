// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "Toolkits/ToolkitManager.h"

#include "AI/SelectionQuery/SQCompositeNode.h"

#define LOCTEXT_NAMESPACE "SelectionQueryEditor"

//////////////////////////////////////////////////////////////////////////

UEdGraphSchema_SelectionQuery::UEdGraphSchema_SelectionQuery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UEdGraphSchema_SelectionQuery::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<USelectionQueryGraphNode_Root> NodeCreator(Graph);
	USelectionQueryGraphNode_Root* MyNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(MyNode, FNodeMetadata::DefaultGraphNode);
}

void UEdGraphSchema_SelectionQuery::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	USelectionQueryGraphNode* ParentGraphNode = ContextMenuBuilder.FromPin ? Cast<USelectionQueryGraphNode>(ContextMenuBuilder.FromPin->GetOuter()) : NULL;
	if  (ParentGraphNode && !ParentGraphNode->IsA(USelectionQueryGraphNode_Root::StaticClass()))
	{
		return;
	}

	FSelectionQueryEditorModule& EditorModule = FModuleManager::GetModuleChecked<FSelectionQueryEditorModule>(TEXT("SelectionQueryEditor"));
	FGraphNodeClassHelper* ClassCache = EditorModule.GetClassCache().Get();
	
	TArray<FGraphNodeClassData> NodeClasses;
	ClassCache->GatherClasses(USQCompositeNode::StaticClass(), NodeClasses);

	FCategorizedGraphActionListBuilder GeneratorsBuilder(TEXT("Composites"));
	for (const auto& NodeClass : NodeClasses)
	{
		const FText NodeTypeName = FText::FromString(FName::NameToDisplayString(NodeClass.ToString(), false));

		USelectionQueryGraphNode_Composite* OpNode = NewObject<USelectionQueryGraphNode_Composite>(ContextMenuBuilder.OwnerOfTemporaries);
		OpNode->ClassData = NodeClass;

		TSharedPtr<FAISchemaAction_NewNode> AddOpAction = AddNewNodeAction(GeneratorsBuilder, NodeClass.GetCategory(), NodeTypeName, "");
		AddOpAction->NodeTemplate = OpNode;
	}

	ContextMenuBuilder.Append(GeneratorsBuilder);
}

void UEdGraphSchema_SelectionQuery::GetSubNodeClasses(int32 SubNodeFlags, TArray<FGraphNodeClassData>& ClassData, UClass*& GraphNodeClass) const
{
}

const FPinConnectionResponse UEdGraphSchema_SelectionQuery::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	// Make sure the pins are not on the same node
	if (PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Both are on the same node"));
	}

	if ((PinA->Direction == EGPD_Input && PinA->LinkedTo.Num()>0) || 
		(PinB->Direction == EGPD_Input && PinB->LinkedTo.Num()>0))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT(""));
	}

	// Compare the directions
	bool bDirectionsOK = false;

	if ((PinA->Direction == EGPD_Input) && (PinB->Direction == EGPD_Output))
	{
		bDirectionsOK = true;
	}
	else if ((PinB->Direction == EGPD_Input) && (PinA->Direction == EGPD_Output))
	{
		bDirectionsOK = true;
	}

	if (bDirectionsOK)
	{
		if ( (PinA->Direction == EGPD_Input && PinA->LinkedTo.Num()>0) || (PinB->Direction == EGPD_Input && PinB->LinkedTo.Num()>0))
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Already connected with other"));
		}
	}
	else
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT(""));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

const FPinConnectionResponse UEdGraphSchema_SelectionQuery::CanMergeNodes(const UEdGraphNode* NodeA, const UEdGraphNode* NodeB) const
{
	// Make sure the nodes are not the same 
	if (NodeA == NodeB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Both are the same node"));
	}

	const bool bNodeAIsItem = NodeA->IsA(USelectionQueryGraphNode_Item::StaticClass());
	const bool bNodeAIsComposite = NodeA->IsA(USelectionQueryGraphNode_Composite::StaticClass());
	const bool bNodeBIsItem = NodeB->IsA(USelectionQueryGraphNode_Item::StaticClass());
	const bool bNodeBIsComposite = NodeB->IsA(USelectionQueryGraphNode_Composite::StaticClass());

	if (bNodeAIsComposite && (bNodeBIsItem || bNodeBIsComposite))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT(""));
}

int32 UEdGraphSchema_SelectionQuery::GetNodeSelectionCount(const UEdGraph* Graph) const
{
	if (Graph)
	{
		TSharedPtr<ISelectionQueryEditor> SelQueryEditor;
		if (USelectionQuery* QueryAsset = Cast<USelectionQuery>(Graph->GetOuter()))
		{
			TSharedPtr< IToolkit > QueryAssetEditor = FToolkitManager::Get().FindEditorForAsset(QueryAsset);
			if (QueryAssetEditor.IsValid())
			{
				SelQueryEditor = StaticCastSharedPtr<ISelectionQueryEditor>(QueryAssetEditor);
			}
		}
		if (SelQueryEditor.IsValid())
		{
			return SelQueryEditor->GetSelectedNodesCount();
		}
	}

	return 0;
}

#undef LOCTEXT_NAMESPACE
