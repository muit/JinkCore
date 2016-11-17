// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SelectionQueryTypes.h"
#include "AI/SelectionQuery/SQResultSourceInterface.h"
#include "SQInstanceBlueprintWrapper.generated.h"

struct FSQRequest;

UCLASS(BlueprintType, meta=(DisplayName="Selection Query Instance"))
class JINKCORE_API USQInstanceBlueprintWrapper : public UObject, public ISQResultSourceInterface
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSQDoneSignature, USQInstanceBlueprintWrapper*, QueryInstance, ESQStatus, QueryStatus);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Selection Query")
	int32 QueryID;

	ESQRunMode RunMode;

	TSharedPtr<FSQResult> QueryResult;
	TSharedPtr<FSelectionQueryInstance> QueryInstance;

    /*
	UPROPERTY(BlueprintReadOnly, Category = "Selection Query")
	TSubclassOf<USQItemType> ItemType;*/

	/** index of query option, that generated items */
	UPROPERTY(BlueprintReadOnly, Category = "Selection Query")
	int32 OptionIndex;
	
	UPROPERTY(BlueprintAssignable, Category = "AI|Selection Query", meta = (DisplayName = "OnQueryFinished"))
	FSQDoneSignature OnQueryFinishedEvent;

public:
	USQInstanceBlueprintWrapper(const FObjectInitializer& ObjectInitializer);
	
	void RunQuery(const ESQRunMode InRunMode, FSQRequest& QueryRequest);

	//UFUNCTION(BlueprintPure, Category = "AI|Selection Query")
	//float GetItemScore(int32 ItemIndex);

protected:
	void OnQueryFinished(TSharedPtr<FSQResult> Result);
};