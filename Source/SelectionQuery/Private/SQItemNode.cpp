// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "SelectionQueryPrivatePCH.h"
#include "SQItemNode.h"

USQItemNode::USQItemNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    NodeName = "Item";
}

void USQItemNode::PostLoad()
{
    Super::PostLoad();
}
