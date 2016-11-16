// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DataProviders/AIDataProvider.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "SelectionQueryTypes.generated.h"

class AActor;
class ARecastNavMesh;
class UNavigationQueryFilter;
class USelectionQuery;
struct FSelectionQueryInstance;
struct FSQOptionInstance;
struct FSQItemDetails;

UENUM()
enum class ESQStatus : uint8
{
    Processing,
    Success,
    Failed,
    Aborted,
    OwnerLost
};

UENUM()
enum class ESQRunMode : uint8
{
    SingleResult UMETA(Tooltip = "Pick one random item", DisplayName = "Single Item")
};

//////////////////////////////////////////////////////////////////////////
// Returned results

USTRUCT()
struct JINKCORE_API FSQResult
{
	GENERATED_USTRUCT_BODY()

protected:
	/** result item */
	UPROPERTY(BlueprintReadOnly, Category = "Selection Query")
	TSubclassOf<UObject> Item;

private:
    /** query status */
    ESQStatus Status;

public:

	/** instance ID */
	UPROPERTY(BlueprintReadOnly, Category = "Selection Query")
	int32 QueryID;

	/** instance owner. Mind that it doesn't have to be the query's "Querier". This is just the object that is responsible for this query instance. */
	TWeakObjectPtr<UObject> Owner;

	//FORCEINLINE float GetItemScore(int32 Index) const { return Items.IsValidIndex(Index) ? Items[Index].Score : 0.0f; }

	/** item accessors for basic types */
	AActor* GetItemAsActor(int32 Index) const;
	FVector GetItemAsLocation(int32 Index) const;

	/** note that this function does not strip out the null-actors to not mess up results of GetItemScore(Index) calls*/
	void GetAllAsActors(TArray<AActor*>& OutActors) const;
	void GetAllAsLocations(TArray<FVector>& OutLocations) const;

	FSQResult() : Status(ESQStatus::Processing) {}
	FSQResult(const ESQStatus& InStatus) : Status(InStatus) {}

	FORCEINLINE bool IsFinished() const { return Status != ESQStatus::Processing; }
	FORCEINLINE bool IsAborted() const { return Status == ESQStatus::Aborted; }
	FORCEINLINE void MarkAsAborted() { Status = ESQStatus::Aborted; }
	FORCEINLINE void MarkAsFailed() { Status = ESQStatus::Failed; }
	FORCEINLINE void MarkAsFinishedWithoutIssues() { Status = ESQStatus::Success; }
	FORCEINLINE void MarkAsOwnerLost() { Status = ESQStatus::OwnerLost; }

	FORCEINLINE ESQStatus GetRawStatus() const { return Status; }
};


//////////////////////////////////////////////////////////////////////////
// Runtime processing structures

DECLARE_DELEGATE_OneParam(FQueryFinishedSignature, TSharedPtr<FSQResult>);

UCLASS(Abstract)
class JINKCORE_API USelectionQueryTypes : public UObject
{
	GENERATED_BODY()

public:
	/** special test value assigned to items skipped by condition check */
	static float SkippedItemValue;

	static FText GetShortTypeName(const UObject* Ob);
};


struct JINKCORE_API FSelectionQueryInstance : public FSQResult
{
    typedef float FNamedParamValueType;

    /** short name of query template - friendly name for debugging */
    FString QueryName;

    /** unique name of query template - object name */
    FName UniqueName;

    /** world owning this query instance */
    UWorld* World;

    /** observer's delegate */
    FQueryFinishedSignature FinishDelegate;

    /** list of options */
    TArray<FSQOptionInstance> Options;

    /** currently processed test (-1 = generator) */
    int32 CurrentTest;

    /** non-zero if test run last step has been stopped mid-process. This indicates
    *	index of the first item that needs processing when resumed */
    int32 CurrentTestStartingItem;

    /** list of item details */
    TArray<FSQItemDetails> ItemDetails;

    /** number of valid items on list */
    int32 NumValidItems;

    /** size of current value */
    uint16 ValueSize;

    /** used to breaking from item iterator loops */
    uint8 bFoundSingleResult : 1;

private:
    /** set when testing final condition of an option */
    uint8 bPassOnSingleResult : 1;

    /** true if this query has logged a warning that it overran the time limit */
    uint8 bHasLoggedTimeLimitWarning : 1;

    /** platform time when this query was started */
    double StartTime;

    /** if > 0 then it's how much time query has for performing current step */
    double CurrentStepTimeLimit;

    /** time spent executing this query */
    double TotalExecutionTime;

    /** time spent doing generation for this query */
    double GenerationExecutionTime;

    // @todo do we really need this data in shipped builds?
    /** time spent on each test of this query */
    TArray<double> PerStepExecutionTime;

public:
    /** run mode */
    ESQRunMode Mode;

    FSelectionQueryInstance();
    FSelectionQueryInstance(const FSelectionQueryInstance& Other);
    ~FSelectionQueryInstance();

    /** execute single step of query */
    void ExecuteOneStep(double InCurrentStepTimeLimit);

    /** set when we started the query */
    void SetQueryStartTime() { StartTime = FPlatformTime::Seconds(); }

    /** get when we started the query */
    double GetQueryStartTime() const { return StartTime; }

    /** have we logged that we overran time the time limit? */
    bool HasLoggedTimeLimitWarning() const { return !!bHasLoggedTimeLimitWarning; }

    /** set that we logged that we overran time the time limit. */
    void SetHasLoggedTimeLimitWarning() { bHasLoggedTimeLimitWarning = true; }

    /** get the amount of time spent executing query */
    double GetTotalExecutionTime() const { return TotalExecutionTime; }

    /** describe for logging purposes what the query spent time on */
    FString GetExecutionTimeDescription() const;

    bool IsInSingleItemFinalSearch() const { return !!bPassOnSingleResult; }
    /** check if current test can batch its calculations */
    bool CanBatchTest() const { return !IsInSingleItemFinalSearch(); }

    /** raw data operations */
    void ReserveItemData(int32 NumAdditionalItems);

    template<typename TypeItem, typename TypeValue>
    void AddItemData(TypeValue ItemValue)
    {
        DEC_MEMORY_STAT_BY(STAT_AI_EQS_InstanceMemory, RawData.GetAllocatedSize() + Items.GetAllocatedSize());

        check(GetDefault<TypeItem>()->GetValueSize() == sizeof(TypeValue));
        check(GetDefault<TypeItem>()->GetValueSize() == ValueSize);
        const int32 DataOffset = RawData.AddUninitialized(ValueSize);
        TypeItem::SetValue(RawData.GetData() + DataOffset, ItemValue);
        Items.Add(FSQItem(DataOffset));

        INC_MEMORY_STAT_BY(STAT_AI_EQS_InstanceMemory, RawData.GetAllocatedSize() + Items.GetAllocatedSize());
    }

    /** AddItemData specialization for arrays if values */
    template<typename TypeItem, typename TypeValue>
    void AddItemData(TArray<TypeValue>& ItemCollection)
    {
        if (ItemCollection.Num() > 0)
        {
            DEC_MEMORY_STAT_BY(STAT_AI_EQS_InstanceMemory, RawData.GetAllocatedSize() + Items.GetAllocatedSize());

            check(GetDefault<TypeItem>()->GetValueSize() == sizeof(TypeValue));
            check(GetDefault<TypeItem>()->GetValueSize() == ValueSize);
            int32 DataOffset = RawData.AddUninitialized(ValueSize * ItemCollection.Num());
            Items.Reserve(Items.Num() + ItemCollection.Num());

            for (TypeValue& Item : ItemCollection)
            {
                TypeItem::SetValue(RawData.GetData() + DataOffset, Item);
                Items.Add(FSQItem(DataOffset));
                DataOffset += ValueSize;
            }

            INC_MEMORY_STAT_BY(STAT_AI_EQS_InstanceMemory, RawData.GetAllocatedSize() + Items.GetAllocatedSize());
        }
    }

protected:

    /** prepare item data after generator has finished */
    void FinalizeGeneration();

    /** update costs and flags after test has finished */
    void FinalizeTest();

    /** final pass on items of finished query */
    void FinalizeQuery();

    /** normalize total score in range 0..1 */
    void NormalizeScores();

    /** sort all scores, from highest to lowest */
    void SortScores();

    /** pick one of items with score equal or higher than specified */
    void PickRandomItemOfScoreAtLeast(float MinScore);

    /** discard all items but one */
    void PickSingleItem(int32 ItemIndex);

public:

    /** removes all runtime data that can be used for debugging (not a part of actual query result) */
    void StripRedundantData();

#if !NO_LOGGING
    void Log(const FString Msg) const;
#endif // #if !NO_LOGGING


#if USE_EQS_DEBUGGER
    FEQSQueryDebugData DebugData;
    static bool bDebuggingInfoEnabled;
#endif // USE_EQS_DEBUGGER

    FBox GetBoundingBox() const;
};
