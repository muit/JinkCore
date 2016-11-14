// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "SelectionQueryManager.generated.h"

class UObject;
class UWorld;
class USelectionQuery;
class USelectionQueryManager;
class USQCompositeNode;

/** wrapper for easy query execution */
USTRUCT()
struct JINKCORE_API FSelQueryRequest
{
	GENERATED_USTRUCT_BODY()

	FSelQueryRequest() : QueryTemplate(NULL), Owner(NULL), World(NULL) {}

	// basic constructor: owner will be taken from finish delegate bindings
	FSelQueryRequest(const USelectionQuery* Query) : QueryTemplate(Query), Owner(NULL), World(NULL) {}

	// use when owner is different from finish delegate binding
	FSelQueryRequest(const USelectionQuery* Query, UObject* RequestOwner) : QueryTemplate(Query), Owner(RequestOwner), World(NULL) {}

	// set world (for accessing query manager) when owner can't provide it
	FORCEINLINE FSelQueryRequest& SetWorldOverride(UWorld* InWorld) { World = InWorld; return *this; }

	template< class UserClass >	
	FORCEINLINE int32 Execute(ESelQueryRunMode::Type Mode, UserClass* InObj, typename FQueryFinishedSignature::TUObjectMethodDelegate< UserClass >::FMethodPtr InMethod)
	{
		return Execute(Mode, FQueryFinishedSignature::CreateUObject(InObj, InMethod));
	}
	template< class UserClass >	
	FORCEINLINE int32 Execute(ESelQueryRunMode::Type Mode, UserClass* InObj, typename FQueryFinishedSignature::TUObjectMethodDelegate_Const< UserClass >::FMethodPtr InMethod)
	{
		return Execute(Mode, FQueryFinishedSignature::CreateUObject(InObj, InMethod));
	}
	int32 Execute(ESelQueryRunMode::Type RunMode, FQueryFinishedSignature const& FinishDelegate);

protected:
	/** query to run */
	UPROPERTY()
	const USelectionQuery* QueryTemplate;

	/** querier */
	UPROPERTY()
	UObject* Owner;

	/** world */
	UPROPERTY()
	UWorld* World;

	friend USelectionQueryManager;
};

UCLASS(config = Game, defaultconfig)
class JINKCORE_API USelectionQueryManager : public UObject, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

	// We need to implement GetWorld() so that any EQS-related blueprints (such as blueprint contexts) can implement
	// GetWorld() and so provide access to blueprint nodes using hidden WorldContextObject parameters.
	virtual UWorld* GetWorld() const override;

	/** [FTickableGameObject] get world function */
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

	/** [FTickableGameObject] tick function */
	virtual void Tick(float DeltaTime) override;

	/** [FTickableGameObject] always tick, unless it's the default object */
	virtual bool IsTickable() const override { return HasAnyFlags(RF_ClassDefaultObject) == false; }

	/** [FTickableGameObject] tick stats */
	virtual TStatId GetStatId() const override;

	/** execute query */
	int32 RunQuery(const FSelQueryRequest& Request, ESelQueryRunMode::Type RunMode, FQueryFinishedSignature const& FinishDelegate);
	int32 RunQuery(const TSharedPtr<FSelQueryInstance>& QueryInstance, FQueryFinishedSignature const& FinishDelegate);

	/** Removed all active queries asked by Querier. No "on finished" notifications are being sent, call this function when
	 *	you no longer care about Querier's queries, like when he's "dead" */
	void SilentlyRemoveAllQueriesByQuerier(const UObject& Querier)
	{
		RemoveAllQueriesByQuerier(Querier, /*bExecuteFinishDelegate=*/false);
	}

	void RemoveAllQueriesByQuerier(const UObject& Querier, bool bExecuteFinishDelegate);

	/** alternative way to run queries. Do not use for anything other than testing
	*  or when you know exactly what you're doing! Bypasses all EQS perf controlling
	*  and time slicing mechanics. */
	TSharedPtr<FSelQueryResult> RunInstantQuery(const FSelQueryRequest& Request, ESelQueryRunMode::Type RunMode);
	void RunInstantQuery(const TSharedPtr<FSelQueryInstance>& QueryInstance);

	/** Creates a query instance configured for execution */
	TSharedPtr<FSelQueryInstance> PrepareQueryInstance(const FSelQueryRequest& Request, ESelQueryRunMode::Type RunMode);

	/** finds USelectionQuery matching QueryName by first looking at instantiated queries (from InstanceCache)
	 *	falling back to looking up all USelectionQuery and testing their name */
	USelectionQuery* FindQueryTemplate(const FString& QueryName) const;

	/** creates local context object */
	USelectionQueryContext* PrepareLocalContext(TSubclassOf<USelectionQueryContext> ContextClass);

	/** execute query */
	bool AbortQuery(int32 RequestID);

	/** fail all running queries on cleaning the world */
	virtual void OnWorldCleanup();

	/** cleanup hooks for map loading */
	virtual void FinishDestroy() override;

	/** add information for data providers about query instance run independently */
	void RegisterExternalQuery(const TSharedPtr<FSelQueryInstance>& QueryInstance);

	/** clear information about query instance run independently */
	void UnregisterExternalQuery(const TSharedPtr<FSelQueryInstance>& QueryInstance);

	/** list of all known item types */
	static TArray<TSubclassOf<USelectionQueryItemType> > RegisteredItemTypes;

	static USelectionQueryManager* GetCurrent(UWorld* World);
	static USelectionQueryManager* GetCurrent(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "AI|Selection Query", meta = (WorldContext = "WorldContext", AdvancedDisplay = "WrapperClass"))
	static USelQueryInstanceBlueprintWrapper* RunSelectionQuery(UObject* WorldContext, USelectionQuery* QueryTemplate, UObject* Querier, TEnumAsByte<EEnvQueryRunMode::Type> RunMode, TSubclassOf<USelQueryInstanceBlueprintWrapper> WrapperClass);

	void RegisterActiveWrapper(USelQueryInstanceBlueprintWrapper& Wrapper);
	void UnregisterActiveWrapper(USelQueryInstanceBlueprintWrapper& Wrapper);

	static void SetAllowTimeSlicing(bool bAllowTimeSlicing);

protected:
	friend USelQueryInstanceBlueprintWrapper;
	TSharedPtr<FSelQueryInstance> FindQueryInstance(const int32 QueryID);

	/** currently running queries */
	TArray<TSharedPtr<FSelQueryInstance> > RunningQueries;

	/** count of queries aborted since last update, to be removed. */
	int32 NumRunningQueriesAbortedSinceLastUpdate;

	/** queries run independently from manager, mapped here for data providers */
	TMap<int32, TWeakPtr<FEnvQueryInstance>> ExternalQueries;

	/** cache of instances */
	UPROPERTY(transient)
	TArray<FEnvQueryInstanceCache> InstanceCache;

	/** local cache of context objects for managing BP based objects */
	UPROPERTY(transient)
	TArray<UEnvQueryContext*> LocalContexts;

	UPROPERTY()
	TArray<UEnvQueryInstanceBlueprintWrapper*> GCShieldedWrappers;

	/** local contexts mapped by class names */
	TMap<FName, UEnvQueryContext*> LocalContextMap;

	/** next ID for running query */
	int32 NextQueryID;

	/** create new instance, using cached data is possible */
	TSharedPtr<FSelQueryInstance> CreateQueryInstance(const USelectionQuery* Template, ESelQueryRunMode::Type RunMode);

	/** whether we update EQS queries based on:
	    running a test on one query and move to the next (breadth) - default behavior,
	    or test an entire query before moving to the next one (depth). */
	UPROPERTY(config)
	bool bTestQueriesUsingBreadth;

	/** if greater than zero, we will warn once when the number of queries is greater than or equal to this number, and log the queries out */
	UPROPERTY(config)
	int32 QueryCountWarningThreshold;

	/** how often (in seconds) we will warn about the number of queries (allows us to catch multiple occurrences in a session) */
	UPROPERTY(config)
	double QueryCountWarningInterval;

private:

	/** create and bind delegates in instance */
	void CreateOptionInstance(USelQueryOption* OptionTemplate, const TArray<UEnvQueryTest*>& SortedTests, FEnvQueryInstance& Instance);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	static bool bAllowEQSTimeSlicing;

	mutable double LastQueryCountWarningThresholdTime;

	void CheckQueryCount() const;
	void LogQueryCountWarning() const;
#endif
};