// Fill out your copyright notice in the Description page of Project Settings.

#include "SelectionQueryPrivatePCH.h"

#include "SQCompositeNode.h"

#include "SelectionQuery.h"

USelectionQuery::USelectionQuery(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USelectionQuery::PostInitProperties()
{
    Super::PostInitProperties();
    QueryName = GetFName();
}

void USelectionQuery::PostLoad()
{
    Super::PostLoad();

    if (QueryName == NAME_None || QueryName.IsValid() == false)
    {
        QueryName = GetFName();
    }
}

#if WITH_EDITOR
void USelectionQuery::PostDuplicate(bool bDuplicateForPIE)
{
    if (bDuplicateForPIE == false)
    {
        QueryName = GetFName();
    }

    Super::PostDuplicate(bDuplicateForPIE);
}
#endif // WITH_EDITOR
