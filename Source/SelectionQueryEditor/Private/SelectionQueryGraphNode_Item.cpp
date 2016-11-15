// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

USelectionQueryGraphNode_Task::USelectionQueryGraphNode_Task(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USelectionQueryGraphNode_Task::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, TEXT("Transition"), TEXT(""), NULL, false, false, TEXT("In"));
}

FText USelectionQueryGraphNode_Task::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	const USQNode* MyNode = Cast<USQNode>(NodeInstance);
	if (MyNode != NULL)
	{
		return FText::FromString(MyNode->GetNodeName());
	}
	else if (!ClassData.GetClassName().IsEmpty())
	{
		FString StoredClassName = ClassData.GetClassName();
		StoredClassName.RemoveFromEnd(TEXT("_C"));

		return FText::Format(NSLOCTEXT("AIGraph", "NodeClassError", "Class {0} not found, make sure it's saved!"), FText::FromString(StoredClassName));
	}

	return Super::GetNodeTitle(TitleType);
}

void USelectionQueryGraphNode_Task::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	//AddContextMenuActionsDecorators(Context);
	//AddContextMenuActionsServices(Context);
}
