// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

USelectionQueryGraphNode_Root::USelectionQueryGraphNode_Root(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsReadOnly = true;
}

void USelectionQueryGraphNode_Root::AllocateDefaultPins()
{
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Transition"), TEXT(""), NULL, false, false, TEXT("In"));
}

FText USelectionQueryGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("SelectionQueryEditor", "Root", "ROOT");
}

void USelectionQueryGraphNode_Root::LogDebugMessage(const FString& Message)
{
	if (DebugMessages.Num() == 0)
	{
		bHasDebugError = false;
	}

	// store only 1 error message, discard everything after it
	if (!bHasDebugError)
	{
		DebugMessages.Add(Message);
	}
}

void USelectionQueryGraphNode_Root::LogDebugError(const FString& Message)
{
	if (DebugMessages.Num() == 0)
	{
		bHasDebugError = false;
	}

	// store only 1 error message, discard everything after it
	if (!bHasDebugError)
	{
		DebugMessages.Add(Message);
		bHasDebugError = true;
	}
}
