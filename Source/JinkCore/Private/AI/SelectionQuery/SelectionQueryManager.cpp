// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AI/SelectionQuery/SelectionQuery.h"
#include "AI/SelectionQuery/SelectionQueryManager.h"
#include "JinkGameMode.h"

#if WITH_EDITOR
#include "UnrealEd.h"
#include "Engine/Brush.h"
#include "EngineUtils.h"

extern UNREALED_API UEditorEngine* GEditor;
#endif // WITH_EDITOR

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
bool USelectionQueryManager::bAllowSQTimeSlicing = true;
#endif

//////////////////////////////////////////////////////////////////////////
// FSQRequest

int32 FSQRequest::Execute(ESQRunMode RunMode, FQueryFinishedSignature const& FinishDelegate)
{
    if (Owner == NULL)
    {
        Owner = FinishDelegate.GetUObject();
        if (Owner == NULL)
        {
            UE_LOG(LogSelectionQuery, Warning, TEXT("Unknown owner of request: %s"), *GetNameSafe(QueryTemplate));
            return INDEX_NONE;
        }
    }

    if (World == NULL)
    {
        World = GEngine->GetWorldFromContextObject(Owner);
        if (World == NULL)
        {
            UE_LOG(LogSelectionQuery, Warning, TEXT("Unable to access world with owner: %s"), *GetNameSafe(Owner));
            return INDEX_NONE;
        }
    }

    USelectionQueryManager* SelQueryManager = USelectionQueryManager::GetCurrent(World);
    if (SelQueryManager == NULL) 
    {
        UE_LOG(LogSelectionQuery, Warning, TEXT("Missing Selection Query manager!"));
        return INDEX_NONE;
    }

    return SelQueryManager->RunQuery(*this, RunMode, FinishDelegate);
}


//////////////////////////////////////////////////////////////////////////
// USelectionQueryManager

TArray<TSubclassOf<USelectionQueryItemType> > USelectionQueryManager::RegisteredItemTypes;

USelectionQueryManager::USelectionQueryManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    NextQueryID = 0;
    MaxAllowedTestingTime = 0.01;
    bTestQueriesUsingBreadth = true;
    NumRunningQueriesAbortedSinceLastUpdate = 0;

    QueryCountWarningThreshold = 0;
    QueryCountWarningInterval = 30.0;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
    LastQueryCountWarningThresholdTime = -FLT_MAX;
#endif
}

UWorld* USelectionQueryManager::GetWorld() const
{
    return Cast<UWorld>(GetOuter());
}

void USelectionQueryManager::FinishDestroy()
{
    FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
    Super::FinishDestroy();
}

USelectionQueryManager* USelectionQueryManager::GetCurrent(UWorld* World)
{
    AJinkGameMode* GM = Cast<AJinkGameMode(World->GetGameMode())
    return GM ? GM->GetSelectionQueryManager() : NULL;
}

USelectionQueryManager* USelectionQueryManager::GetCurrent(const UObject* WorldContextObject)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, false);
    return /*AISys ? AISys->GetSelectionQueryManager() : */NULL;
}

TStatId USelectionQueryManager::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(USelectionQueryManager, STATGROUP_Tickables);
}

int32 USelectionQueryManager::RunQuery(const FSQRequest& Request, ESQRunMode RunMode, FQueryFinishedSignature const& FinishDelegate)
{
    TSharedPtr<FSelectionQueryInstance> QueryInstance = PrepareQueryInstance(Request, RunMode);
    return RunQuery(QueryInstance, FinishDelegate);
}

int32 USelectionQueryManager::RunQuery(const TSharedPtr<FSelectionQueryInstance>& QueryInstance, FQueryFinishedSignature const& FinishDelegate)
{
    if (QueryInstance.IsValid() == false)
    {
        return INDEX_NONE;
    }

    QueryInstance->FinishDelegate = FinishDelegate;
    QueryInstance->SetQueryStartTime();
    RunningQueries.Add(QueryInstance);

    return QueryInstance->QueryID;
}

TSharedPtr<FSQResult> USelectionQueryManager::RunInstantQuery(const FSQRequest& Request, ESQRunMode RunMode)
{
    TSharedPtr<FSelectionQueryInstance> QueryInstance = PrepareQueryInstance(Request, RunMode);
    if (!QueryInstance.IsValid())
    {
        return NULL;
    }

    RunInstantQuery(QueryInstance);

    return QueryInstance;
}

void USelectionQueryManager::RunInstantQuery(const TSharedPtr<FSelectionQueryInstance>& QueryInstance)
{
    if (!ensure(QueryInstance.IsValid()))
    {
        return;
    }

    QueryInstance->SetQueryStartTime();
    RegisterExternalQuery(QueryInstance);
    while (QueryInstance->IsFinished() == false)
    {
        QueryInstance->ExecuteOneStep((double)FLT_MAX);
    }

    UnregisterExternalQuery(QueryInstance);

    UE_VLOG_EQS(*QueryInstance.Get(), LogEQS, All);

#if USE_EQS_DEBUGGER
    EQSDebugger.StoreQuery(GetWorld(), QueryInstance);
#endif // USE_EQS_DEBUGGER
}

void USelectionQueryManager::RemoveAllQueriesByQuerier(const UObject& Querier, bool bExecuteFinishDelegate)
{
    for (int32 QueryIndex = RunningQueries.Num() - 1; QueryIndex >= 0; --QueryIndex)
    {
        const TSharedPtr<FSelectionQueryInstance>& QueryInstance = RunningQueries[QueryIndex];
        if (QueryInstance.IsValid() == false || QueryInstance->Owner.IsValid() == false || QueryInstance->Owner.Get() == &Querier)
        {
            if (QueryInstance->IsFinished() == false)
            {
                QueryInstance->MarkAsAborted();

                if (bExecuteFinishDelegate)
                {
                    QueryInstance->FinishDelegate.ExecuteIfBound(QueryInstance);
                }

                // We will remove the aborted query from the RunningQueries array on the next EQS update
                ++NumRunningQueriesAbortedSinceLastUpdate;
            }
        }
    }
}

TSharedPtr<FSelectionQueryInstance> USelectionQueryManager::PrepareQueryInstance(const FSQRequest& Request, ESQRunMode RunMode)
{
    TSharedPtr<FSelectionQueryInstance> QueryInstance = CreateQueryInstance(Request.QueryTemplate, RunMode);
    if (!QueryInstance.IsValid())
    {
        return NULL;
    }

    QueryInstance->World = Cast<UWorld>(GetOuter());
    QueryInstance->Owner = Request.Owner;

    DEC_MEMORY_STAT_BY(STAT_AI_EQS_InstanceMemory, QueryInstance->NamedParams.GetAllocatedSize());

    // @TODO: interface for providing default named params (like custom ranges in AI)
    QueryInstance->NamedParams = Request.NamedParams;

    INC_MEMORY_STAT_BY(STAT_AI_EQS_InstanceMemory, QueryInstance->NamedParams.GetAllocatedSize());

    QueryInstance->QueryID = NextQueryID++;

    return QueryInstance;
}

bool USelectionQueryManager::AbortQuery(int32 RequestID)
{
    for (int32 QueryIndex = 0; QueryIndex < RunningQueries.Num(); QueryIndex++)
    {
        TSharedPtr<FSelectionQueryInstance>& QueryInstance = RunningQueries[QueryIndex];
        if (QueryInstance->QueryID == RequestID &&
            QueryInstance->IsFinished() == false)
        {
            QueryInstance->MarkAsAborted();
            QueryInstance->FinishDelegate.ExecuteIfBound(QueryInstance);

            // We will remove the aborted query from the RunningQueries array on the next EQS update
            ++NumRunningQueriesAbortedSinceLastUpdate;

            return true;
        }
    }

    return false;
}

void USelectionQueryManager::Tick(float DeltaTime)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
    CheckQueryCount();
#endif

    SCOPE_CYCLE_COUNTER(STAT_AI_EQS_Tick);
    SET_DWORD_STAT(STAT_AI_EQS_NumInstances, RunningQueries.Num());

    double TimeLeft = MaxAllowedTestingTime;

    int32 QueriesFinishedDuringUpdate = 0;
    const double ExecutionTimeWarningSeconds = 0.25;

    {
        SCOPE_CYCLE_COUNTER(STAT_AI_EQS_TickWork);

        const int32 NumRunningQueries = RunningQueries.Num();
        int32 Index = 0;
        while ((TimeLeft > 0.0) && (Index < NumRunningQueries) && (QueriesFinishedDuringUpdate < NumRunningQueries))
        {
            const double StartTime = FPlatformTime::Seconds();
            double QuerierHandlingDuration = 0.0;

            const TSharedPtr<FSelectionQueryInstance>& QueryInstance = RunningQueries[Index];

            if (QueryInstance->IsFinished())
            {
                // If this query is already finished, skip it.
                ++Index;
            }
            else
            {
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
                if (!bAllowEQSTimeSlicing)
                {
                    // Passing in -1 causes QueryInstance to set its Deadline to -1, which in turn causes it to 
                    // never fail based on time input.  (In fact, it's odd that we use FLT_MAX in RunInstantQuery(),
                    // since that could simply use -1. as well.)  Note: "-1." to explicitly specify that it's a double.
                    QueryInstance->ExecuteOneStep(-1.);
                }
                else
#endif
                {
                    QueryInstance->ExecuteOneStep(TimeLeft);
                }

                if (QueryInstance->IsFinished())
                {
                    // Always log that we executed total execution time at the end of the query.
                    if (QueryInstance->GetTotalExecutionTime() > ExecutionTimeWarningSeconds)
                    {
                        UE_LOG(LogEQS, Warning, TEXT("Finished query %s over execution time warning. %s"), *QueryInstance->QueryName, *QueryInstance->GetExecutionTimeDescription());
                    }

                    // Now, handle the response to the query finishing, but calculate the time from that to remove from
                    // the time spent for time-slicing purposes, because that's NOT the EQS manager doing work.
                    {
                        SCOPE_CYCLE_COUNTER(STAT_AI_EQS_TickNotifies);
                        double QuerierHandlingStartTime = FPlatformTime::Seconds();

                        UE_VLOG_EQS(*QueryInstance.Get(), LogEQS, All);

#if USE_EQS_DEBUGGER
                        EQSDebugger.StoreQuery(GetWorld(), QueryInstance);
#endif // USE_EQS_DEBUGGER

                        QueryInstance->FinishDelegate.ExecuteIfBound(QueryInstance);

                        QuerierHandlingDuration = FPlatformTime::Seconds() - QuerierHandlingStartTime;
                    }

                    ++QueriesFinishedDuringUpdate;
                    ++Index;
                }
                // If we're testing queries using breadth, move on to the next query.
                // If we're testing queries using depth, we only move on to the next query when we finish the current one.
                else if (bTestQueriesUsingBreadth)
                {
                    ++Index;
                }

                if (!QueryInstance->HasLoggedTimeLimitWarning() && (QueryInstance->GetTotalExecutionTime() > ExecutionTimeWarningSeconds))
                {
                    UE_LOG(LogEQS, Warning, TEXT("Query %s over execution time warning. %s"), *QueryInstance->QueryName, *QueryInstance->GetExecutionTimeDescription());
                    QueryInstance->SetHasLoggedTimeLimitWarning();
                }
            }

            // Start over at the beginning if we are testing using breadth and we've reached the end of the list
            if (bTestQueriesUsingBreadth && (Index == NumRunningQueries))
            {
                Index = 0;
            }

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
            if (bAllowEQSTimeSlicing) // if Time slicing is enabled...
#endif
            {	// Don't include the querier handling as part of the total time spent by EQS for time-slicing purposes.
                TimeLeft -= ((FPlatformTime::Seconds() - StartTime) - QuerierHandlingDuration);
            }
        }
    }

    {
        const int32 NumQueriesFinished = QueriesFinishedDuringUpdate + NumRunningQueriesAbortedSinceLastUpdate;
        double FinishedQueriesTotalTime(0.0);

        if (NumQueriesFinished > 0)
        {
            SCOPE_CYCLE_COUNTER(STAT_AI_EQS_TickQueryRemovals);

            // When using breadth testing we don't know when a particular query will finish,
            // or if we have queries that were aborted since the last update we don't know which ones were aborted,
            // so we have to go through all the queries.
            // When doing depth without any queries aborted since the last update we know how many to remove.
            // Or if we have finished all the queries.  In that case we don't need to check if the queries are finished)
            if ((NumQueriesFinished != RunningQueries.Num()) && (bTestQueriesUsingBreadth || (NumRunningQueriesAbortedSinceLastUpdate > 0)))
            {
                for (int32 Index = RunningQueries.Num() - 1, FinishedQueriesCounter = NumQueriesFinished; Index >= 0 && FinishedQueriesCounter > 0; --Index)
                {
                    TSharedPtr<FSelectionQueryInstance>& QueryInstance = RunningQueries[Index];

                    if (QueryInstance->IsFinished())
                    {
                        FinishedQueriesTotalTime += FPlatformTime::Seconds() - QueryInstance->GetQueryStartTime();

                        RunningQueries.RemoveAt(Index, 1, /*bAllowShrinking=*/false);

                        --FinishedQueriesCounter;
                    }
                }
            }
            else // queries tested using depth without any aborted queries since our last update, or we're removing all queries
            {
                for (int32 Index = 0; Index < NumQueriesFinished; ++Index)
                {
                    TSharedPtr<FSelectionQueryInstance>& QueryInstance = RunningQueries[Index];
                    ensure(QueryInstance->IsFinished());

                    FinishedQueriesTotalTime += FPlatformTime::Seconds() - QueryInstance->GetQueryStartTime();
                }

                RunningQueries.RemoveAt(0, NumQueriesFinished, /*bAllowShrinking=*/false);
            }
        }

        // Reset the running queries aborted since last update counter
        NumRunningQueriesAbortedSinceLastUpdate = 0;

        double InstanceAverageResponseTime = 0.f;
        if (NumQueriesFinished > 0)
        {
            // Convert to ms from seconds
            InstanceAverageResponseTime = FinishedQueriesTotalTime / (double)NumQueriesFinished * 1000.0;
        }

        SET_FLOAT_STAT(STAT_AI_EQS_AvgInstanceResponseTime, InstanceAverageResponseTime);
    }
}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
void USelectionQueryManager::CheckQueryCount() const
{
    if ((QueryCountWarningThreshold > 0) && (RunningQueries.Num() >= QueryCountWarningThreshold))
    {
        const double CurrentTime = FPlatformTime::Seconds();

        if ((LastQueryCountWarningThresholdTime < 0.0) || ((LastQueryCountWarningThresholdTime + QueryCountWarningInterval) < CurrentTime))
        {
            LogQueryCountWarning();

            LastQueryCountWarningThresholdTime = CurrentTime;
        }
    }
}

void USelectionQueryManager::LogQueryCountWarning() const
{
    UE_LOG(LogEQS, Warning, TEXT("The number of EQS queries has reached (%d) the warning threshold (%d).  Logging queries."), RunningQueries.Num(), QueryCountWarningThreshold);

    for (const TSharedPtr<FSelectionQueryInstance>& RunningQuery : RunningQueries)
    {
        if (RunningQuery.IsValid())
        {
            UE_LOG(LogEQS, Warning, TEXT("Query: %s - Owner: %s"), *RunningQuery->QueryName, RunningQuery->Owner.IsValid() ? *RunningQuery->Owner->GetName() : TEXT("Invalid"));
        }
        else
        {
            UE_LOG(LogEQS, Warning, TEXT("Invalid query found in list!"));
        }
    }
}
#endif

void USelectionQueryManager::OnWorldCleanup()
{
    if (RunningQueries.Num() > 0)
    {
        // @todo investigate if this is even needed. We should be fine with just removing all queries
        TArray<TSharedPtr<FSelectionQueryInstance> > RunningQueriesCopy = RunningQueries;
        RunningQueries.Reset();

        for (int32 Index = 0; Index < RunningQueriesCopy.Num(); Index++)
        {
            TSharedPtr<FSelectionQueryInstance>& QueryInstance = RunningQueriesCopy[Index];
            if (QueryInstance->IsFinished() == false)
            {
                QueryInstance->MarkAsFailed();
                QueryInstance->FinishDelegate.ExecuteIfBound(QueryInstance);
            }
        }
    }

    GCShieldedWrappers.Reset();
}

void USelectionQueryManager::RegisterExternalQuery(const TSharedPtr<FSelectionQueryInstance>& QueryInstance)
{
    if (QueryInstance.IsValid())
    {
        ExternalQueries.Add(QueryInstance->QueryID, QueryInstance);
    }
}

void USelectionQueryManager::UnregisterExternalQuery(const TSharedPtr<FSelectionQueryInstance>& QueryInstance)
{
    if (QueryInstance.IsValid())
    {
        ExternalQueries.Remove(QueryInstance->QueryID);
    }
}

namespace SelQueryTestSort
{
    struct FAllMatching
    {
        FORCEINLINE bool operator()(const USelectionQueryTest& TestA, const USelectionQueryTest& TestB) const
        {
            // cheaper tests go first
            if (TestB.Cost > TestA.Cost)
            {
                return true;
            }

            // conditions go first
            const bool bConditionA = (TestA.TestPurpose != ESelTestPurpose::Score); // Is Test A Filtering?
            const bool bConditionB = (TestB.TestPurpose != ESelTestPurpose::Score); // Is Test B Filtering?
            if (bConditionA && !bConditionB)
            {
                return true;
            }

            // keep connection order (sort stability)
            return (TestB.TestOrder > TestA.TestOrder);
        }
    };

    struct FSingleResult
    {
        FSingleResult(ESelTestCost::Type InHighestCost) : HighestCost(InHighestCost) {}

        FORCEINLINE bool operator()(const USelectionQueryTest& TestA, const USelectionQueryTest& TestB) const
        {
            // cheaper tests go first
            if (TestB.Cost > TestA.Cost)
            {
                return true;
            }

            const bool bConditionA = (TestA.TestPurpose != ESelTestPurpose::Score); // Is Test A Filtering?
            const bool bConditionB = (TestB.TestPurpose != ESelTestPurpose::Score); // Is Test B Filtering?
            if (TestA.Cost == HighestCost)
            {
                // highest cost: weights go first, conditions later (first match will return result)
                if (!bConditionA && bConditionB)
                {
                    return true;
                }
            }
            else
            {
                // lower costs: conditions go first to reduce amount of items
                if (bConditionA && !bConditionB)
                {
                    return true;
                }
            }

            // keep connection order (sort stability)
            return (TestB.TestOrder > TestA.TestOrder);
        }

    protected:
        TEnumAsByte<ESelTestCost::Type> HighestCost;
    };
}

USelectionQuery* USelectionQueryManager::FindQueryTemplate(const FString& QueryName) const
{
    for (int32 InstanceIndex = 0; InstanceIndex < InstanceCache.Num(); InstanceIndex++)
    {
        const USelectionQuery* QueryTemplate = InstanceCache[InstanceIndex].Template;

        if (QueryTemplate != NULL && QueryTemplate->GetName() == QueryName)
        {
            return const_cast<USelectionQuery*>(QueryTemplate);
        }
    }

    for (FObjectIterator It(USelectionQuery::StaticClass()); It; ++It)
    {
        if (It->GetName() == QueryName)
        {
            return Cast<USelectionQuery>(*It);
        }
    }

    return NULL;
}

TSharedPtr<FSelectionQueryInstance> USelectionQueryManager::CreateQueryInstance(const USelectionQuery* Template, ESQRunMode RunMode)
{
    if (Template == nullptr || Template->Options.Num() == 0)
    {
        UE_CLOG(Template != nullptr && Template->Options.Num() == 0, LogEQS, Warning, TEXT("Query [%s] doesn't have any valid options!"), *Template->GetName());
        return nullptr;
    }

    // try to find entry in cache
    FSelectionQueryInstance* InstanceTemplate = NULL;
    for (int32 InstanceIndex = 0; InstanceIndex < InstanceCache.Num(); InstanceIndex++)
    {
        if (InstanceCache[InstanceIndex].Template->GetFName() == Template->GetFName() &&
            InstanceCache[InstanceIndex].Instance.Mode == RunMode)
        {
            InstanceTemplate = &InstanceCache[InstanceIndex].Instance;
            break;
        }
    }

    // and create one if can't be found
    if (InstanceTemplate == NULL)
    {
        SCOPE_CYCLE_COUNTER(STAT_AI_EQS_LoadTime);

        // duplicate template in manager's world for BP based nodes
        USelectionQuery* LocalTemplate = (USelectionQuery*)StaticDuplicateObject(Template, this, TEXT("None"));

        {
            // memory stat tracking: temporary variable will exist only inside this section
            FSelectionQueryInstanceCache NewCacheEntry;
            NewCacheEntry.Template = LocalTemplate;
            NewCacheEntry.Instance.UniqueName = LocalTemplate->GetFName();
            NewCacheEntry.Instance.QueryName = LocalTemplate->GetQueryName().ToString();
            NewCacheEntry.Instance.Mode = RunMode;

            const int32 Idx = InstanceCache.Add(NewCacheEntry);
            InstanceTemplate = &InstanceCache[Idx].Instance;
        }

        // NOTE: We must iterate over this from 0->Num because we are copying the options from the template into the
        // instance, and order matters!  Since we also may need to remove invalid or null options, we must decrement
        // the iteration pointer when doing so to avoid problems.
        for (int32 OptionIndex = 0; OptionIndex < LocalTemplate->Options.Num(); ++OptionIndex)
        {
            USelectionQueryOption* MyOption = LocalTemplate->Options[OptionIndex];
            if (MyOption == nullptr ||
                MyOption->Generator == nullptr ||
                MyOption->Generator->ItemType == nullptr)
            {
                UE_LOG(LogEQS, Error, TEXT("Trying to spawn a query with broken Template (generator:%s itemType:%s): %s, option %d"),
                    MyOption ? (MyOption->Generator ? TEXT("ok") : TEXT("MISSING")) : TEXT("N/A"),
                    (MyOption && MyOption->Generator) ? (MyOption->Generator->ItemType ? TEXT("ok") : TEXT("MISSING")) : TEXT("N/A"),
                    *GetNameSafe(LocalTemplate), OptionIndex);

                LocalTemplate->Options.RemoveAt(OptionIndex, 1, false);
                --OptionIndex; // See note at top of for loop.  We cannot iterate backwards here.
                continue;
            }

            USelectionQueryOption* LocalOption = (USelectionQueryOption*)StaticDuplicateObject(MyOption, this);
            USelectionQueryGenerator* LocalGenerator = (USelectionQueryGenerator*)StaticDuplicateObject(MyOption->Generator, this);
            LocalTemplate->Options[OptionIndex] = LocalOption;
            LocalOption->Generator = LocalGenerator;

            ESelTestCost::Type HighestCost(ESelTestCost::Low);
            TArray<USelectionQueryTest*> SortedTests = MyOption->Tests;
            TSubclassOf<USelectionQueryItemType> GeneratedType = MyOption->Generator->ItemType;
            for (int32 TestIndex = SortedTests.Num() - 1; TestIndex >= 0; TestIndex--)
            {
                USelectionQueryTest* TestOb = SortedTests[TestIndex];
                if (TestOb == NULL || !TestOb->IsSupportedItem(GeneratedType))
                {
                    UE_LOG(LogEQS, Warning, TEXT("Query [%s] can't use test [%s] in option %d [%s], removing it"),
                        *GetNameSafe(LocalTemplate), *GetNameSafe(TestOb), OptionIndex, *MyOption->Generator->OptionName);

                    SortedTests.RemoveAt(TestIndex, 1, false);
                }
                else if (HighestCost < TestOb->Cost)
                {
                    HighestCost = TestOb->Cost;
                }
            }

            LocalOption->Tests.Reset(SortedTests.Num());
            for (int32 TestIdx = 0; TestIdx < SortedTests.Num(); TestIdx++)
            {
                USelectionQueryTest* LocalTest = (USelectionQueryTest*)StaticDuplicateObject(SortedTests[TestIdx], this);
                LocalOption->Tests.Add(LocalTest);
            }

            // use locally referenced duplicates
            SortedTests = LocalOption->Tests;

            if (SortedTests.Num() && LocalGenerator->bAutoSortTests)
            {
                switch (RunMode)
                {
                case ESQRunMode::SingleResult:
                    SortedTests.Sort(SelQueryTestSort::FSingleResult(HighestCost));
                    break;
                case ESQRunMode::AllMatching:
                    SortedTests.Sort(SelQueryTestSort::FAllMatching());
                    break;

                default:
                {
                    UEnum* RunModeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESQRunMode"));
                    UE_LOG(LogSelectionQuery, Warning, TEXT("Query [%s] can't be sorted for RunMode: %d [%s]"),
                        *GetNameSafe(LocalTemplate), (int32)RunMode, RunModeEnum ? *RunModeEnum->GetEnumName(RunMode) : TEXT("??"));
                }
                }
            }

            CreateOptionInstance(LocalOption, SortedTests, *InstanceTemplate);
        }
    }

    if (InstanceTemplate->Options.Num() == 0)
    {
        return nullptr;
    }

    // create new instance
    TSharedPtr<FSelectionQueryInstance> NewInstance(new FSelectionQueryInstance(*InstanceTemplate));
    return NewInstance;
}

void USelectionQueryManager::CreateOptionInstance(USelectionQueryOption* OptionTemplate, const TArray<USelectionQueryTest*>& SortedTests, FSelectionQueryInstance& Instance)
{
    FSelQueryOptionInstance OptionInstance;
    OptionInstance.Generator = OptionTemplate->Generator;
    OptionInstance.ItemType = OptionTemplate->Generator->ItemType;

    OptionInstance.Tests.AddZeroed(SortedTests.Num());
    for (int32 TestIndex = 0; TestIndex < SortedTests.Num(); TestIndex++)
    {
        USelectionQueryTest* TestOb = SortedTests[TestIndex];
        OptionInstance.Tests[TestIndex] = TestOb;
    }

    DEC_MEMORY_STAT_BY(STAT_AI_EQS_InstanceMemory, Instance.Options.GetAllocatedSize());

    const int32 AddedIdx = Instance.Options.Add(OptionInstance);

    INC_MEMORY_STAT_BY(STAT_AI_EQS_InstanceMemory, Instance.Options.GetAllocatedSize() + Instance.Options[AddedIdx].GetAllocatedSize());
}

float USelectionQueryManager::FindNamedParam(int32 QueryId, FName ParamName) const
{
    float ParamValue = 0.0f;

    const TWeakPtr<FSelectionQueryInstance>* QueryInstancePtr = ExternalQueries.Find(QueryId);
    if (QueryInstancePtr)
    {
        TSharedPtr<FSelectionQueryInstance> QueryInstance = (*QueryInstancePtr).Pin();
        if (QueryInstance.IsValid())
        {
            ParamValue = QueryInstance->NamedParams.FindRef(ParamName);
        }
    }
    else
    {
        for (int32 QueryIndex = 0; QueryIndex < RunningQueries.Num(); QueryIndex++)
        {
            const TSharedPtr<FSelectionQueryInstance>& QueryInstance = RunningQueries[QueryIndex];
            if (QueryInstance->QueryID == QueryId)
            {
                ParamValue = QueryInstance->NamedParams.FindRef(ParamName);
                break;
            }
        }
    }

    return ParamValue;
}

//----------------------------------------------------------------------//
// BP functions and related functionality 
//----------------------------------------------------------------------//
USelectionQueryInstanceBlueprintWrapper* USelectionQueryManager::RunEQSQuery(UObject* WorldContext, USelectionQuery* QueryTemplate, UObject* Querier, TEnumAsByte<ESQRunMode::Type> RunMode, TSubclassOf<USelectionQueryInstanceBlueprintWrapper> WrapperClass)
{
    if (QueryTemplate == nullptr || Querier == nullptr)
    {
        return nullptr;
    }

    USelectionQueryManager* EQSManager = GetCurrent(WorldContext);
    USelectionQueryInstanceBlueprintWrapper* QueryInstanceWrapper = nullptr;

    if (EQSManager)
    {
        bool bValidQuerier = true;

        // convert controller-owners to pawns, unless specifically configured not to do so
        if (GET_AI_CONFIG_VAR(bAllowControllersAsEQSQuerier) == false && Cast<AController>(Querier))
        {
            AController* Controller = Cast<AController>(Querier);
            if (Controller->GetPawn())
            {
                Querier = Controller->GetPawn();
            }
            else
            {
                UE_VLOG(Controller, LogEQS, Error, TEXT("Trying to run EQS query while not having a pawn! Aborting."));
                bValidQuerier = false;
            }
        }

        if (bValidQuerier)
        {
            QueryInstanceWrapper = NewObject<USelectionQueryInstanceBlueprintWrapper>(EQSManager, (UClass*)(WrapperClass) ? (UClass*)WrapperClass : USelectionQueryInstanceBlueprintWrapper::StaticClass());
            check(QueryInstanceWrapper);

            FSQRequest QueryRequest(QueryTemplate, Querier);
            // @todo named params still missing support
            //QueryRequest.SetNamedParams(QueryParams);
            QueryInstanceWrapper->RunQuery(RunMode, QueryRequest);
        }
    }

    return QueryInstanceWrapper;
}

void USelectionQueryManager::RegisterActiveWrapper(USelectionQueryInstanceBlueprintWrapper& Wrapper)
{
    GCShieldedWrappers.AddUnique(&Wrapper);
}

void USelectionQueryManager::UnregisterActiveWrapper(USelectionQueryInstanceBlueprintWrapper& Wrapper)
{
    GCShieldedWrappers.RemoveSingleSwap(&Wrapper, /*bAllowShrinking=*/false);
}

TSharedPtr<FSelectionQueryInstance> USelectionQueryManager::FindQueryInstance(const int32 QueryID)
{
    if (QueryID != INDEX_NONE)
    {
        // going from the back since it's most probably there
        for (int32 QueryIndex = RunningQueries.Num() - 1; QueryIndex >= 0; --QueryIndex)
        {
            if (RunningQueries[QueryIndex]->QueryID == QueryID)
            {
                return RunningQueries[QueryIndex];
            }
        }
    }

    return nullptr;
}

//----------------------------------------------------------------------//
// Exec functions (i.e. console commands)
//----------------------------------------------------------------------//
void USelectionQueryManager::SetAllowTimeSlicing(bool bAllowTimeSlicing)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
    bAllowEQSTimeSlicing = bAllowTimeSlicing;

    UE_LOG(LogEQS, Log, TEXT("Set allow time slicing to %s."),
        bAllowEQSTimeSlicing ? TEXT("true") : TEXT("false"));
#else
    UE_LOG(LogEQS, Log, TEXT("Time slicing cannot be disabled in Test or Shipping builds.  SetAllowTimeSlicing does nothing."));
#endif
}
