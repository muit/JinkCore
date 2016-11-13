// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AI/SelectionQuery/SelectionQuery.h"

USelectionQuery::USelectionQuery(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UEnvQuery::PostInitProperties()
{
	Super::PostInitProperties();
	QueryName = GetFName();
}

void UEnvQuery::PostLoad()
{
	Super::PostLoad();

	if (QueryName == NAME_None || QueryName.IsValid() == false)
	{
		QueryName = GetFName();
	}
}

#if WITH_EDITOR
void UEnvQuery::PostDuplicate(bool bDuplicateForPIE)
{
	if (bDuplicateForPIE == false)
	{
		QueryName = GetFName();
	}

	Super::PostDuplicate(bDuplicateForPIE);
}
#endif // WITH_EDITOR