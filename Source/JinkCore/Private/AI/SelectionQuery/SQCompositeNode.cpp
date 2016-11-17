// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AI/SelectionQuery/SQCompositeNode.h"

USQCompositeNode::USQCompositeNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "UnknownComposite";
}

void USQCompositeNode::PostLoad()
{
	Super::PostLoad();
	UpdateNodeVersion();
}
