// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryPrivatePCH.h"
#include "SQItemNode.h"

USQItemNode::USQItemNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USQItemNode::PostLoad()
{
    Super::PostLoad();
}

FString GetNodeName() {
    return "Item";
}
