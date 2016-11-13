// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "SelectionQuery/SQCompositeNode.h"

USQCompositeNode::USQCompositeNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "UnknownComposite";
	bUseChildExecutionNotify = false;
	bUseNodeActivationNotify = false;
	bUseNodeDeactivationNotify = false;
}

UEnvQueryGenerator::UEnvQueryGenerator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bAutoSortTests = true;
}

void UEnvQueryGenerator::UpdateNodeVersion()
{
	VerNum = EnvQueryGeneratorVersion::Latest;
}

void UEnvQueryGenerator::PostLoad()
{
	Super::PostLoad();
	UpdateNodeVersion();
}
