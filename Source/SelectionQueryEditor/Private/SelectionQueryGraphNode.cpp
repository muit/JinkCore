// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

USelectionQueryGraphNode::USelectionQueryGraphNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FText USelectionQueryGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::GetEmpty();
}

FText USelectionQueryGraphNode::GetDescription() const
{
	return FText::GetEmpty();
}

USelectionQueryGraph* USelectionQueryGraphNode::GetSelectionQueryGraph()
{
	return CastChecked<USelectionQueryGraph>(GetGraph());
}

bool USelectionQueryGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const
{
	return DesiredSchema->GetClass()->IsChildOf(UEdGraphSchema_SelectionQuery::StaticClass());
}
