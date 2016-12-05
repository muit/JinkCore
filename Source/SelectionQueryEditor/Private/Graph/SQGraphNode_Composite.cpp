// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "ScopedTransaction.h"

#include "SQCompositeNode.h"

#include "SQGraphNode_Composite.h"

#define LOCTEXT_NAMESPACE "SelectionQueryEditor"

USQGraphNode_Composite::USQGraphNode_Composite(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USQGraphNode_Composite::AllocateDefaultPins()
{
	CreatePin(EGPD_Input,  TEXT("Transition"), TEXT(""), NULL, false, false, TEXT("In"));
	CreatePin(EGPD_Output, TEXT("Transition"), TEXT(""), NULL, false, false, TEXT("Out"));
}

FText USQGraphNode_Composite::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(NodeName.Len() ? NodeName : "[INVALID]");
}

void USQGraphNode_Composite::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
}

#undef LOCTEXT_NAMESPACE
