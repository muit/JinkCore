// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryPrivatePCH.h"
#include "SQCompositeNode.h"

USQCompositeNode::USQCompositeNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "UnknownComposite";
}

void USQCompositeNode::PostLoad()
{
	Super::PostLoad();
}
