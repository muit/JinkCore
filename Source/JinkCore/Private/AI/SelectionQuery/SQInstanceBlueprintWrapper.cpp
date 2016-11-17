// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AI/SelectionQuery/SQInstanceBlueprintWrapper.h"
#include "AI/SelectionQuery/SelectionQueryManager.h"

USQInstanceBlueprintWrapper::USQInstanceBlueprintWrapper(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, QueryID(INDEX_NONE)
{

}

void USQInstanceBlueprintWrapper::OnQueryFinished(TSharedPtr<FSQResult> Result)
{
	check(Result.IsValid());
	QueryResult = Result;

	OnQueryFinishedEvent.Broadcast(this, Result->GetRawStatus());

	// remove our reference to the query instance
	QueryInstance = nullptr;

	// unregister self, no longer shielded from GC
	USelectionQueryManager* SQManager = Cast<USelectionQueryManager>(GetOuter());
	if (ensure(SQManager))
	{
		SQManager->UnregisterActiveWrapper(*this);
	}
}

/*
float USQInstanceBlueprintWrapper::GetItemScore(int32 ItemIndex)
{
	return QueryResult.IsValid() ? QueryResult->GetItemScore(ItemIndex) : -1.f;
}*/

void USQInstanceBlueprintWrapper::RunQuery(const ESQRunMode InRunMode, FSQRequest& QueryRequest)
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
