// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SelectionQuery/Items/EnvQueryItemType.h"
#include "SelectionQuery/EnvQueryContext.h"
#include "DataProviders/AIDataProvider.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "EnvQueryTypes.generated.h"

class AActor;
class ARecastNavMesh;
class UNavigationQueryFilter;
class USelectionQuery;
struct FSelectionQueryInstance;
struct FSQOptionInstance;
struct FSQItemDetails;

//////////////////////////////////////////////////////////////////////////
// Returned results


template <> struct TIsZeroConstructType<FEnvQueryItem> { enum { Value = true }; };

USTRUCT()
struct AIMODULE_API FSQResult
{
	GENERATED_USTRUCT_BODY()

	TArray<FEnvQueryItem> Items;

	/** type of generated items */
	UPROPERTY(BlueprintReadOnly, Category = "EQS")
	TSubclassOf<UEnvQueryItemType> ItemType;

	/** raw data of items */
	TArray<uint8> RawData;

private:
	/** query status */
	EEnvQueryStatus::Type Status;

public:
	/** index of query option, that generated items */
	UPROPERTY(BlueprintReadOnly, Category = "EQS")
	int32 OptionIndex;

	/** instance ID */
	UPROPERTY(BlueprintReadOnly, Category = "EQS")
	int32 QueryID;

	/** instance owner. Mind that it doesn't have to be the query's "Querier". This is just the object that is responsible for this query instance. */
	TWeakObjectPtr<UObject> Owner;

	FORCEINLINE float GetItemScore(int32 Index) const { return Items.IsValidIndex(Index) ? Items[Index].Score : 0.0f; }

	/** item accessors for basic types */
	AActor* GetItemAsActor(int32 Index) const;
	FVector GetItemAsLocation(int32 Index) const;

	/** note that this function does not strip out the null-actors to not mess up results of GetItemScore(Index) calls*/
	void GetAllAsActors(TArray<AActor*>& OutActors) const;
	void GetAllAsLocations(TArray<FVector>& OutLocations) const;

	FEnvQueryResult() : ItemType(NULL), Status(EEnvQueryStatus::Processing), OptionIndex(0) {}
	FEnvQueryResult(const EEnvQueryStatus::Type& InStatus) : ItemType(NULL), Status(InStatus), OptionIndex(0) {}

	FORCEINLINE bool IsFinished() const { return Status != EEnvQueryStatus::Processing; }
	FORCEINLINE bool IsAborted() const { return Status == EEnvQueryStatus::Aborted; }
	FORCEINLINE void MarkAsMissingParam() { Status = EEnvQueryStatus::MissingParam; }
	FORCEINLINE void MarkAsAborted() { Status = EEnvQueryStatus::Aborted; }
	FORCEINLINE void MarkAsFailed() { Status = EEnvQueryStatus::Failed; }
	FORCEINLINE void MarkAsFinishedWithoutIssues() { Status = EEnvQueryStatus::Success; }
	FORCEINLINE void MarkAsOwnerLost() { Status = EEnvQueryStatus::OwnerLost; }

	FORCEINLINE EEnvQueryStatus::Type GetRawStatus() const { return Status; }
};


//////////////////////////////////////////////////////////////////////////
// Runtime processing structures

DECLARE_DELEGATE_OneParam(FQueryFinishedSignature, TSharedPtr<FSQResult>);


struct FEQSQueryDebugData
{
	TArray<FEnvQueryItem> DebugItems;
	TArray<FEnvQueryItemDetails> DebugItemDetails;
	TArray<uint8> RawData;
	TArray<FString> PerformedTestNames;
	// indicates the query was run in a single-item mode and that it has been found
	uint32 bSingleItemResult : 1;

	void Store(const FEnvQueryInstance* QueryInstance);
	void Reset()
	{
		DebugItems.Reset();
		DebugItemDetails.Reset();
		RawData.Reset();
		PerformedTestNames.Reset();
		bSingleItemResult = false;
	}
};


UCLASS(Abstract)
class JINKCORE_API USelecectionQueryTypes : public UObject
{
	GENERATED_BODY()

public:
	/** special test value assigned to items skipped by condition check */
	static float SkippedItemValue;

	static FText GetShortTypeName(const UObject* Ob);
	static FText DescribeContext(TSubclassOf<UEnvQueryContext> ContextClass);
};
