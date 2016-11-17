// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AI/SelectionQuery/SelectionQuery.h"
#include "AI/SelectionQuery/SelectionQueryManager.h"
#include "AI/SelectionQuery/SQInstanceBlueprintWrapper.h"
#include "JinkGameMode.h"

#if WITH_EDITOR
#include "UnrealEd.h"
#include "Engine/Brush.h"
#include "EngineUtils.h"

extern UNREALED_API UEditorEngine* GEditor;
#endif // WITH_EDITOR

//////////////////////////////////////////////////////////////////////////
// FSQRequest

int32 FSQRequest::Execute(ESQRunMode RunMode, FQueryFinishedSignature const& FinishDelegate)
{
    if (Owner == NULL)
    {
        Owner = FinishDelegate.GetUObject();
        if (Owner == NULL)
        {
            //UE_LOG(LogSelectionQuery, Warning, TEXT("Unknown owner of request: %s"), *GetNameSafe(QueryTemplate));
            return INDEX_NONE;
        }
    }

    if (World == NULL)
    {
        World = GEngine->GetWorldFromContextObject(Owner);
        if (World == NULL)
        {
            //UE_LOG(LogSelectionQuery, Warning, TEXT("Unable to access world with owner: %s"), *GetNameSafe(Owner));
            return INDEX_NONE;
        }
    }

    USelectionQueryManager* SelQueryManager = USelectionQueryManager::GetCurrent(World);
    if (SelQueryManager == NULL) 
    {
        //UE_LOG(LogSelectionQuery, Warning, TEXT("Missing Selection Query manager!"));
        return INDEX_NONE;
    }

    return SelQueryManager->RunQuery(*this, RunMode, FinishDelegate);
}


//////////////////////////////////////////////////////////////////////////
// USelectionQueryManager

USelectionQueryManager::USelectionQueryManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    NextQueryID = 0;
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
    if (!World) {
        return NULL;
    }
    AJinkGameMode* GM = Cast<AJinkGameMode>(World->GetAuthGameMode());
    return GM ? GM->GetSelectionQueryManager() : NULL;
}

USelectionQueryManager* USelectionQueryManager::GetCurrent(const UObject* WorldContextObject)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, false);
    return GetCurrent(World);
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

    int32 QueriesFinishedDuringUpdate = 0;
    const double ExecutionTimeWarningSeconds = 0.25;

    {
        const int32 NumRunningQueries = RunningQueries.Num();
        int32 Index = 0;
        while (/*(TimeLeft > 0.0) && */(Index < NumRunningQueries) && (QueriesFinishedDuringUpdate < NumRunningQueries))
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
                if (!bAllowSQTimeSlicing)
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
                        UE_LOG(LogSelectionQuery, Warning, TEXT("Finished query %s over execution time warning. %s"), *QueryInstance->QueryName, *QueryInstance->GetExecutionTimeDescription());
                    }

                    // Now, handle the response to the query finishing, but calculate the time from that to remove from
                    // the time spent for time-slicing purposes, because that's NOT the EQS manager doing work.
                    {
                        double QuerierHandlingStartTime = FPlatformTime::Seconds();

                        //UE_VLOG_SQ(*QueryInstance.Get(), LogSelectionQuery, All);

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
                    UE_LOG(LogSelectionQuery, Warning, TEXT("Query %s over execution time warning. %s"), *QueryInstance->QueryName, *QueryInstance->GetExecutionTimeDescription());
                    QueryInstance->SetHasLoggedTimeLimitWarning();
                }
            }

            // Start over at the beginning if we are testing using breadth and we've reached the end of the list
            if (bTestQueriesUsingBreadth && (Index == NumRunningQueries))
            {
                Index = 0;
            }

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
            if (bAllowSQTimeSlicing) // if Time slicing is enabled...
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
    UE_LOG(LogSelectionQuery, Warning, TEXT("The number of selection queries has reached (%d) the warning threshold (%d).  Logging queries."), RunningQueries.Num(), QueryCountWarningThreshold);

    for (const TSharedPtr<FSelectionQueryInstance>& RunningQuery : RunningQueries)
    {
        if (RunningQuery.IsValid())
        {
            UE_LOG(LogSelectionQuery, Warning, TEXT("Query: %s - Owner: %s"), *RunningQuery->QueryName, RunningQuery->Owner.IsValid() ? *RunningQuery->Owner->GetName() : TEXT("Invalid"));
        }
        else
        {
            UE_LOG(LogSelectionQuery, Warning, TEXT("Invalid query found in list!"));
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
    if (Template == nullptr)
    {
        UE_LOG(LogSelectionQuery, Warning, TEXT("Query [%s] doesn't have a valid template!"), *Template->GetName());
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
        // duplicate template in manager's world for BP based nodes
        USelectionQuery* LocalTemplate = (USelectionQuery*)StaticDuplicateObject(Template, this, TEXT("None"));

        {
            // memory stat tracking: temporary variable will exist only inside this section
            FSQInstanceCache NewCacheEntry;
            NewCacheEntry.Template = LocalTemplate;
            NewCacheEntry.Instance.UniqueName = LocalTemplate->GetFName();
            NewCacheEntry.Instance.QueryName = LocalTemplate->GetQueryName().ToString();
            NewCacheEntry.Instance.Mode = RunMode;

            const int32 Idx = InstanceCache.Add(NewCacheEntry);
            InstanceTemplate = &InstanceCache[Idx].Instance;
        }
    }

    // create new instance
    TSharedPtr<FSelectionQueryInstance> NewInstance(new FSelectionQueryInstance(*InstanceTemplate));
    return NewInstance;
}

//----------------------------------------------------------------------//
// BP functions and related functionality 
//----------------------------------------------------------------------//
USQInstanceBlueprintWrapper* USelectionQueryManager::RunQuery(UObject* WorldContext, USelectionQuery* QueryTemplate, UObject* Querier, TEnumAsByte<ESQRunMode> RunMode, TSubclassOf<USQInstanceBlueprintWrapper> WrapperClass)
{
    if (QueryTemplate == nullptr || Querier == nullptr)
    {
        return nullptr;
    }

    USelectionQueryManager* SQManager = GetCurrent(WorldContext);
    USQInstanceBlueprintWrapper* QueryInstanceWrapper = nullptr;

    if (SQManager)
    {
        bool bValidQuerier = true;

        // convert controller-owners to pawns, unless specifically configured not to do so
        if (Cast<AController>(Querier))
        {
            AController* Controller = Cast<AController>(Querier);
            if (Controller->GetPawn())
            {
                Querier = Controller->GetPawn();
            }
            else
            {
                //UE_VLOG(Controller, LogSelectionQuery, Error, TEXT("Trying to run EQS query while not having a pawn! Aborting."));
                bValidQuerier = false;
            }
        }

        if (bValidQuerier)
        {
            QueryInstanceWrapper = NewObject<USQInstanceBlueprintWrapper>(SQManager, (UClass*)(WrapperClass) ? (UClass*)WrapperClass : USQInstanceBlueprintWrapper::StaticClass());
            check(QueryInstanceWrapper);

            FSQRequest QueryRequest(QueryTemplate, Querier);
            // @todo named params still missing support
            //QueryRequest.SetNamedParams(QueryParams);
            QueryInstanceWrapper->RunQuery(RunMode, QueryRequest);
        }
    }

    return QueryInstanceWrapper;
}

void USelectionQueryManager::RegisterActiveWrapper(USQInstanceBlueprintWrapper& Wrapper)
{
    GCShieldedWrappers.AddUnique(&Wrapper);
}

void USelectionQueryManager::UnregisterActiveWrapper(USQInstanceBlueprintWrapper& Wrapper)
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
    bAllowSQTimeSlicing = bAllowTimeSlicing;

    UE_LOG(LogSelectionQuery, Log, TEXT("Set allow time slicing to %s."),
        bAllowSQTimeSlicing ? TEXT("true") : TEXT("false"));
#else
    UE_LOG(LogSelectionQuery, Log, TEXT("Time slicing cannot be disabled in Test or Shipping builds.  SetAllowTimeSlicing does nothing."));
#endif
}
