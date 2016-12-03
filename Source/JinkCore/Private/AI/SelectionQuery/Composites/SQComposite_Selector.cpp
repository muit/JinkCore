// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AI/SelectionQuery/Composites/SQComposite_Selector.h"

USQComposite_Selector::USQComposite_Selector(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Selector";
}

#if WITH_EDITOR

FName USQComposite_Selector::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Composite.Selector.Icon");
}

#endif