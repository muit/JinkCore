// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

#include "EdGraph_SelectionQuery.h"
//#include "EdGraphNode_------.h"
#include "SQConnectionDrawingPolicy.h"

#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraph.h"
//#include "EdGraphSchema_Extensions.h"

#include "EdGraphSchema_SelectionQuery.h"


#define LOCTEXT_NAMESPACE "EdGraphSchema_SelectionQuery"
#define SNAP_GRID (16) // @todo ensure this is the same as SNodePanel::GetSnapGridSize()

namespace
{
	// Maximum distance a drag can be off a node edge to require 'push off' from node
	const int32 NodeDistance = 60;
}

UEdGraphNode* FSQSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	UEdGraphNode* ResultNode = NULL;

	// If there is a template, we actually use it
	if (NodeTemplate != NULL)
	{
		NodeTemplate->SetFlags(RF_Transactional);

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(NULL, ParentGraph, REN_NonTransactional);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		
		// For input pins, new node will generally overlap node being dragged off
		// Work out if we want to visually push away from connected node
		int32 XLocation = Location.X;
		if (FromPin && FromPin->Direction == EGPD_Input)
		{
			UEdGraphNode* PinNode = FromPin->GetOwningNode();
			const float XDelta = FMath::Abs(PinNode->NodePosX - Location.X);

			if (XDelta < NodeDistance)
			{
				// Set location to edge of current node minus the max move distance
				// to force node to push off from connect node enough to give selection handle
				XLocation = PinNode->NodePosX - NodeDistance;
			}
		}

		NodeTemplate->NodePosX = XLocation;
		NodeTemplate->NodePosY = Location.Y;
		NodeTemplate->SnapToGrid(SNAP_GRID);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

UEdGraphNode* FSQSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	UEdGraphNode* ResultNode = NULL;

	if (FromPins.Num() > 0)
	{
		ResultNode = PerformAction(ParentGraph, FromPins[0], Location, bSelectNewNode);

		// Try autowiring the rest of the pins
		for (int32 Index = 1; Index < FromPins.Num(); ++Index)
		{
			ResultNode->AutowireNewNode(FromPins[Index]);
		}
	}
	else
	{
		ResultNode = PerformAction(ParentGraph, NULL, Location, bSelectNewNode);
	}

	return ResultNode;
}

void FSQSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);

	// These don't get saved to disk, but we want to make sure the objects don't get GC'd while the action array is around
	Collector.AddReferencedObject(NodeTemplate);
}
/////////////////////////////////////////////////////////

UEdGraphSchema_SelectionQuery::UEdGraphSchema_SelectionQuery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UEdGraphSchema_SelectionQuery::GetActionList(TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions, const UEdGraph* Graph, UEdGraph* OwnerOfTemporaries) const
{
    //TODO: Add Node Creation actions

	//UEdGraphSchema_Extensions::Get().CreateCustomActions(OutActions, Graph, OwnerOfTemporaries);
}


void UEdGraphSchema_SelectionQuery::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	FFormatNamedArguments Args;
	const FName AttrName("Attributes");
	Args.Add(TEXT("Attribute"), FText::FromName(AttrName));
	const UEdGraphPin* FromPin = ContextMenuBuilder.FromPin;

	if (FromPin) {
        //Check Action types
	}

	const UEdGraph* Graph = ContextMenuBuilder.CurrentGraph;
	TArray<TSharedPtr<FEdGraphSchemaAction> > Actions;
	GetActionList(Actions, Graph, ContextMenuBuilder.OwnerOfTemporaries);

	for (TSharedPtr<FEdGraphSchemaAction> Action : Actions) {
		ContextMenuBuilder.AddAction(Action);
	}
}

FString GetNodeName(const UEdGraphNode* Node) {
	FText emitterText = Node->GetNodeTitle(ENodeTitleType::FullTitle);
	return emitterText.ToString();
}
/*
TArray<USQGraphNode*> GetOutgoingNodes(const UEdGraphPin* A, const UEdGraphPin* B, const USQGraphNode* Node) {
	TArray<USQGraphNode*> Result;
	UEdGraphPin* OutPin = Node->GetOutputPin();
	if (OutPin) {
		for (const UEdGraphPin* InputPin : OutPin->LinkedTo) {
			USQGraphNode* NextNode = Cast<USQGraphNode>(InputPin->GetOwningNode());
			if (NextNode) {
				Result.Add(NextNode);
			}
		}
	}

	if (OutPin == A) {
        USQGraphNode* NextNode = Cast<USQGraphNode>(B->GetOwningNode());
		if (NextNode) Result.Add(NextNode);
	}
	if (OutPin == B) {
        USQGraphNode* NextNode = Cast<USQGraphNode>(A->GetOwningNode());
		if (NextNode) Result.Add(NextNode);
	}
	return Result;
}*/
/*TArray<FString> GetItems(const UEdGraphPin* A, const UEdGraphPin* B, const UEdGraphNode_Composite* CompositeNode) {
	TArray<FString> ItemNames;
	TArray<USQGraphNode*> Nodes = GetOutgoingNodes(A, B, CompositeNode);

	for (const USQGraphNode* MeshNode : MeshNodes) {
		TArray<USQGraphNode*> EmitterNodes = GetOutgoingNodes(A, B, MeshNode);
		for (const USQGraphNode* EmitterNode : EmitterNodes) {
			EmitterNames.Add(GetNodeName(EmitterNode));
		}
	}
	return EmitterNames;
}*/

typedef TMap<FString, TArray<FString>> CompositeToEmitterMapping_t;

bool CheckCycleRecursive(const CompositeToEmitterMapping_t& CompositeToEmitterMapping, TArray<FString>& TraversePath) {
	FString TopComposite = TraversePath.Last();
	if (!CompositeToEmitterMapping.Contains(TopComposite)) return false;
	const TArray<FString>& Emitters = CompositeToEmitterMapping[TopComposite];
	for (const FString Emitter : Emitters) {
		// Check if this emitter forms a cycle with the existing traverse path
		if (TraversePath.Contains(Emitter)) {
			TraversePath.Add(Emitter);
			return true;
		}
		TraversePath.Add(Emitter);
		bool bContainsCycle = CheckCycleRecursive(CompositeToEmitterMapping, TraversePath);
		if (bContainsCycle) {
			return true;
		}
		// Remove the last element
		TraversePath.RemoveAt(TraversePath.Num() - 1);
	}
	return false;
}

bool FindCycles(const CompositeToEmitterMapping_t& CompositeToEmitterMapping, TArray<FString>& OutCyclePath) {
	TSet<FString> CompositeVisited;
	TArray<FString> Composites;
	CompositeToEmitterMapping.GenerateKeyArray(Composites);

	for (const FString& CompositeName : Composites) {
		if (CompositeVisited.Contains(CompositeName)) {
			// Already processed
			continue;
		}

		// Start a DFS from here and see if we come back to a visited node
		TArray<FString> TraversePath;
		TraversePath.Add(CompositeName);
		if (CheckCycleRecursive(CompositeToEmitterMapping, TraversePath)) {
			OutCyclePath = TraversePath;
			return true;
		}
	}

	return false;
}
/*
bool UEdGraphSchema_SelectionQuery::ContainsCycles(const UEdGraphPin* A, const UEdGraphPin* B, TArray<FString>& OutCyclePath) const
{
	if (!A || !B) return false;
	UEdGraph* Graph = A->GetOwningNode()->GetGraph();

	TArray<UEdGraphNode_Composite*> CompositeNodes;
	Graph->GetNodesOfClass<UEdGraphNode_Composite>(CompositeNodes);

	
	CompositeToEmitterMapping_t CompositeToEmitterMapping;
	for (const UEdGraphNode_Composite* CompositeNode : CompositeNodes) {
		TArray<FString> Items = GetItems(A, B, CompositeNode);
		FString CompositeName = GetNodeName(CompositeNode);
		if (!CompositeToEmitterMapping.Contains(CompopsiteName)) {
			CompositeToEmitterMapping.Add(CompositeName, Items);
		}
	}

	// Check if we find a cycle

	OutCyclePath = TArray<FString>();
	if (FindCycles(CompositeToEmitterMapping, OutCyclePath)) {
		return true;
	}

	return false;
}*/

FString Combine(const TArray<FString> Array, FString Separator) {
	// TODO: Use a string builder
	FString Result;
	for (const FString& Item : Array) {
		if (Result.Len() > 0) {
			Result += Separator;
		}
		Result += Item;
	}
	return Result;
}

const FPinConnectionResponse UEdGraphSchema_SelectionQuery::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Make sure the input is connecting to an output
	if (A->Direction == B->Direction) {
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not allowed"));
	}

	// Make sure the data types match
	if (A->PinType.PinCategory != B->PinType.PinCategory) {
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not allowed"));
	}
    /*
	// Make sure we don't have a cycle formed by this link
	TArray<FString> CyclePath;
	if (ContainsCycles(A, B, CyclePath)) {
		FString CycleString = Combine(CyclePath, " > ");
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not allowed. Contains a cycle: " + CycleString));
	}
    */
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

class FConnectionDrawingPolicy* UEdGraphSchema_SelectionQuery::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FSQConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

FLinearColor UEdGraphSchema_SelectionQuery::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::Yellow;
}

bool UEdGraphSchema_SelectionQuery::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	return false;
}


bool UEdGraphSchema_SelectionQuery::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	bool ConnectionMade = UEdGraphSchema::TryCreateConnection(A, B);
	if (ConnectionMade) {
		UEdGraphPin* OutputPin = (A->Direction == EEdGraphPinDirection::EGPD_Output) ? A : B;
        /*
		USQGraphNode* OutputNode = Cast<USQGraphNode>(OutputPin->GetOwningNode());
		if (OutputNode) {
			OutputNode->UpdateChildExecutionOrder();
			OutputNode->GetGraph()->NotifyGraphChanged();
		}*/
	}

	return ConnectionMade;
}

#undef LOCTEXT_NAMESPACE
