// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

#include "SQNode.h"
#include "EdGraphSchema_SelectionQuery.h"

USQGraphNode::USQGraphNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

//~ Begin UEdGraphNode Interface
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

UEdGraph_SelectionQuery* USQGraphNode::GetSQGraph()
{
	return CastChecked<UEdGraph_SelectionQuery>(GetGraph());
}

bool USQGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const
{
	return DesiredSchema->GetClass()->IsChildOf(UEdGraphSchema_SelectionQuery::StaticClass());
}
