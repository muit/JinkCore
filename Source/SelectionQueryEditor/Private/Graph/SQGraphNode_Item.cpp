// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

USQGraphNode_Item::USQGraphNode_Item(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USQGraphNode_Item::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, TEXT("Transition"), TEXT(""), NULL, false, false, TEXT("In"));
}

FText USQGraphNode_Item::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return FText::FromString(NodeName.Len()? NodeName : "[INVALID]");
}

void USQGraphNode_Item::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	//AddContextMenuActionsDecorators(Context);
	//AddContextMenuActionsServices(Context);
}
