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
        QueryInstance->ExecuteOneStep();
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

USelectionQuery* USelectionQueryManager::FindQueryTemplate(const FString& QueryName) const
{
    for (FObjectIterator It(USelectionQuery::StaticClass()); It; ++It)
    {
        if (It->GetName() == QueryName)
        {
            return Cast<USelectionQuery>(*It);
        }
    }

    return NULL;
}

TSharedPtr<FSelectionQueryInstance> USelectionQueryManager::CreateQueryInstance(const USelectionQuery* QueryTemplate, ESQRunMode RunMode)
{
    if (QueryTemplate == nullptr)
    {
        UE_LOG(LogSelectionQuery, Warning, TEXT("Query [%s] doesn't have a valid template!"), *Template->GetName());
        return nullptr;
    }

    FSelectionQueryInstance* InstanceTemplate = NULL;

    // duplicate template in manager's world for BP based nodes
    USelectionQuery* LocalQueryTemplate = (USelectionQuery*)StaticDuplicateObject(QueryTemplate, this, TEXT("None"));

    InstanceTemplate->UniqueName = LocalQueryTemplate->GetFName();
    InstanceTemplate->QueryName = LocalQueryTemplate->GetQueryName().ToString();
    InstanceTemplate->Mode = RunMode;

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
