// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "EnvironmentQuery/SelectionQueryManager.h"
#include "SelectionQuery/SQItemNode.h"

USQItemNode::USQItemNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USQItemNode::UpdateNodeVersion()
{
	VerNum = EnvQueryGeneratorVersion::Latest;
}

void USQItemNode::PostLoad()
{
	Super::PostLoad();
	UpdateNodeVersion();
}
