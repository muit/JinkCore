// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AI/SelectionQuery/SQInstanceBlueprintWrapper.h"
#include "AI/SelectionQuery/SQManager.h"

USQInstanceBlueprintWrapper::USQInstanceBlueprintWrapper(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, QueryID(INDEX_NONE)
{

}

void USQInstanceBlueprintWrapper::OnQueryFinished(TSharedPtr<FSQResult> Result)
{
	check(Result.IsValid());
	QueryResult = Result;
	ItemType = Result->ItemType;
	OptionIndex = Result->OptionIndex;

	OnQueryFinishedEvent.Broadcast(this, Result->GetRawStatus());

	// remove our reference to the query instance
	QueryInstance = nullptr;

	// unregister self, no longer shielded from GC
	USQManager* SQManager = Cast<USQManager>(GetOuter());
	if (ensure(SQManager))
	{
		SQManager->UnregisterActiveWrapper(*this);
	}
}

float USQInstanceBlueprintWrapper::GetItemScore(int32 ItemIndex)
{
	return QueryResult.IsValid() ? QueryResult->GetItemScore(ItemIndex) : -1.f;
}

TArray<AActor*> USQInstanceBlueprintWrapper::GetResultsAsActors()
{
	TArray<AActor*> Results;

	if (QueryResult.IsValid() && ItemType->IsChildOf(USQItemType_ActorBase::StaticClass()))
	{
		if (RunMode != ESQRunMode::AllMatching)
		{
			Results.Add(QueryResult->GetItemAsActor(0));
		}
		else
		{
			QueryResult->GetAllAsActors(Results);
		}
	}

	return Results;
}

TArray<FVector> USQInstanceBlueprintWrapper::GetResultsAsLocations()
{
	TArray<FVector> Results;

	if (QueryResult.IsValid())
	{
		if (RunMode != ESQRunMode::AllMatching)
		{
			Results.Add(QueryResult->GetItemAsLocation(0));
		}
		else
		{
			QueryResult->GetAllAsLocations(Results);
		}
	}

	return Results;
}

void USQInstanceBlueprintWrapper::RunQuery(const ESQRunMode::Type InRunMode, FSQRequest& QueryRequest)
{
	RunMode = InRunMode;
	QueryID = QueryRequest.Execute(RunMode, this, &USQInstanceBlueprintWrapper::OnQueryFinished);
	if (QueryID != INDEX_NONE)
	{
		// register self as a wrapper needing shielding from GC
		USelectionQueryManager* SQManager = Cast<USelectionQueryManager>(GetOuter());
		if (ensure(SQManager))
		{
			SQManager->RegisterActiveWrapper(*this);
			QueryInstance = SQManager->FindQueryInstance(QueryID);
		}
	}
}

void USQInstanceBlueprintWrapper::SetNamedParam(FName ParamName, float Value)
{
	FSQInstance* InstancePtr = QueryInstance.Get();
	if (InstancePtr != nullptr)
	{
		InstancePtr->NamedParams.Add(ParamName, Value);
	}
}