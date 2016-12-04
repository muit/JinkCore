// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

#include "EdGraph_SelectionQuery.h"
//#include "EdGraphSchema_SelectionQuery.h"

#define SNAP_GRID (16) // @todo ensure this is the same as SNodePanel::GetSnapGridSize()

#define LOCTEXT_NAMESPACE "SelectionQueryGraph"

const FString FSelectionQueryDataTypes::PinType_Mesh = "mesh";
const FString FSelectionQueryDataTypes::PinType_Marker = "marker";

UEdGraph_SelectionQuery::UEdGraph_SelectionQuery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Schema = UEdGraphSchema_SelectionQuery::StaticClass();
}
/*
template<typename T>
void GetChildNodes(USQGraphNode* ParentNode, TArray<T*>& OutChildren) {
	for (UEdGraphPin* ChildPin : ParentNode->GetOutputPin()->LinkedTo) {
		if (ChildPin) {
			if (T* DesiredNode = Cast<T>(ChildPin->GetOwningNode())) {
				OutChildren.Add(DesiredNode);
			}
		}
	}
}*/

/*
struct ExecutionSortComparer {
	inline bool operator() (const UEdGraphNode_DungeonActorBase& A, const UEdGraphNode_DungeonActorBase& B) const {
		return A.ExecutionOrder < B.ExecutionOrder;
	}
};*/

void UEdGraph_SelectionQuery::RebuildGraph(USelectionQuery* SQ,/* TArray<FPropTypeData>& OutProps,*/ TArray<FSQGraphBuildError>& OutErrors)
{
	//OutProps.Reset();
	
	// TODO: Check for cycles

    /* Enable with Root node class
	UEdGraphNode_Root* Root;
	GetNodeOfClass<UEdGraphNode_Root>(Root);
	for (UEdGraphNode_Root* MarkerNode : MarkerNodes) {
		TArray<UEdGraphNode_DungeonActorBase*> ActorNodes;
		ActorNodes.Sort(ExecutionSortComparer());
	}*/
}

void UEdGraph_SelectionQuery::InitializeGraph()
{
    //Create Root Node
    /*{
		UEdGraphNode_Root* RootNode = NewObject<UEdGraphNode_Root>(this);
		RootNode->bUserDefined = false;
        RootNode->Rename(NULL, this, REN_NonTransactional);
        RootNode->MarkerName = "Root";
		this->AddNode(RootNode, true, false);

        RootNode->CreateNewGuid();
        RootNode->PostPlacedNewNode();
        RootNode->AllocateDefaultPins();

        RootNode->NodePosX = 0;
        RootNode->NodePosY = 0;
        RootNode->SnapToGrid(SNAP_GRID);
	}*/
}

FDelegateHandle UEdGraph_SelectionQuery::AddOnNodePropertyChangedHandler(const FOnGraphChanged::FDelegate& InHandler)
{
	return OnNodePropertyChanged.Add(InHandler);
}

void UEdGraph_SelectionQuery::RemoveOnNodePropertyChangedHandler(FDelegateHandle Handle)
{
	OnNodePropertyChanged.Remove(Handle);
}

void UEdGraph_SelectionQuery::NotifyNodePropertyChanged(const FEdGraphEditAction& InAction)
{
	OnNodePropertyChanged.Broadcast(InAction);
}

#undef LOCTEXT_NAMESPACE
