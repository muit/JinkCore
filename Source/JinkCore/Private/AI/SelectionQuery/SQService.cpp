// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "SelectionQuery/SQService.h"

USQService::USQService(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
	bNotifyOnSearch = true;
	SQickIntervals = true;
	bCallTickOnSearchStart = false;
	bRestartTimerOnEachActivation = false;

	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void USQService::TickNode(USelectionQueryComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ScheduleNextTick(NodeMemory);
}

void USQService::OnSearchStart(FSelectionQuerySearchData& SearchData)
{
	// empty in base class
}

void USQService::NotifyParentActivation(FSelectionQuerySearchData& SearchData)
{
	if (bNotifyOnSearch || bNotifyTick)
	{
		USQNode* NodeOb = bCreateNodeInstance ? GetNodeInstance(SearchData) : this;
		if (NodeOb)
		{
			USQService* ServiceNodeOb = (USQService*)NodeOb;
			uint8* NodeMemory = GetNodeMemory<uint8>(SearchData);

			if (bNotifyTick)
			{
				const float RemainingTime = bRestartTimerOnEachActivation ? 0.0f : GetNextTickRemainingTime(NodeMemory);
				if (RemainingTime <= 0.0f)
				{
					ServiceNodeOb->ScheduleNextTick(NodeMemory);
				}
			}

			if (bNotifyOnSearch)
			{
				ServiceNodeOb->OnSearchStart(SearchData);
			}

			if (bCallTickOnSearchStart)
			{
				ServiceNodeOb->TickNode(SearchData.OwnerComp, NodeMemory, 0.0f);
			}
		}
	}
}

FString USQService::GetStaticTickIntervalDescription() const
{
	FString IntervalDesc = (RandomDeviation > 0.0f) ?
		FString::Printf(TEXT("%.2fs..%.2fs"), FMath::Max(0.0f, Interval - RandomDeviation), (Interval + RandomDeviation)) :
		FString::Printf(TEXT("%.2fs"), Interval);

	return FString::Printf(TEXT("tick every %s"), *IntervalDesc);
}

FString USQService::GetStaticServiceDescription() const
{
	return GetStaticTickIntervalDescription();
}

FString USQService::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %s"), *USelectionQueryTypes::GetShortTypeName(this), *GetStaticServiceDescription());
}

#if WITH_EDITOR

FName USQService::GetNodeIconName() const
{
	return FName("SQEditor.Graph.SQNode.Service.Icon");
}

#endif // WITH_EDITOR

void USQService::ScheduleNextTick(uint8* NodeMemory)
{
	const float NextTickTime = FMath::FRandRange(FMath::Max(0.0f, Interval - RandomDeviation), (Interval + RandomDeviation));
	SetNextTickTime(NodeMemory, NextTickTime);
}
