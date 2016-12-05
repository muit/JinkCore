// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"

USQGraphNode_Root::USQGraphNode_Root(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsReadOnly = true;
}

void USQGraphNode_Root::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, TEXT("Transition"), TEXT(""), NULL, false, false, TEXT("In"));
}

FText USQGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("SelectionQueryEditor", "Root", "ROOT");
}
/*
FName USQGraphNode_Root::GetNameIcon() const
{
	return FName("BTEditor.Graph.BTNode.Root.Icon");
}*/
