// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

#include "SQNode.h"
#include "EdGraphSchema_SelectionQuery.h"

USQGraphNode::USQGraphNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    NodeInstance = nullptr;
    bIsReadOnly = false;
}

void USQGraphNode::InitializeInstance()
{
    // empty in base class
}

//~ Begin UEdGraphNode Interface
void USQGraphNode::PostPlacedNewNode() {
    UClass* NodeClass = ClassData.GetClass(true);
    if (NodeClass)
    {
        UEdGraph* MyGraph = GetSQGraph();
        UObject* GraphOwner = MyGraph ? MyGraph->GetOuter() : nullptr;
        if (GraphOwner)
        {
            NodeInstance = NewObject<UObject>(GraphOwner, NodeClass);
            NodeInstance->SetFlags(RF_Transactional);
            InitializeInstance();
        }
    }
}

bool USQGraphNode::CanDuplicateNode() const
{
    return bIsReadOnly ? false : Super::CanDuplicateNode();
}

bool USQGraphNode::CanUserDeleteNode() const
{
    return bIsReadOnly ? false : Super::CanUserDeleteNode();
}
//~ End UEdGraphNode Interface


void USQGraphNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
    if (PropertyObserver.IsValid()) {
        FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
        PropertyObserver->OnPropertyChanged(this, PropertyName);
    }

    Super::PostEditChangeProperty(e);
}

void USQGraphNode::NodeConnectionListChanged()
{
    UEdGraphNode::NodeConnectionListChanged();

    GetGraph()->NotifyGraphChanged();
}

FText USQGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const {
    return FText::GetEmpty();
}

FText USQGraphNode::GetDescription() const {
    return FText::GetEmpty();
}

UEdGraph_SelectionQuery* USQGraphNode::GetSQGraph()
{
    return CastChecked<UEdGraph_SelectionQuery>(GetGraph());
}

bool USQGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const
{
	return DesiredSchema->GetClass()->IsChildOf(UEdGraphSchema_SelectionQuery::StaticClass());
}
