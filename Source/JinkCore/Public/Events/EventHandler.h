// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "EventHandler.generated.h"

DECLARE_DELEGATE_OneParam(FEventDelegate, int);

/**
 * 
 */
UCLASS()
class JINKCORE_API UEventHandler : public UObject
{
	GENERATED_BODY()

public:
	UEventHandler();
	UEventHandler(int Id);
	void Setup(UObject* Context);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	int Id;


	UObject* WorldContext;
	// Handle to manage the timer
	FTimerHandle TimerHandle;
	//Delegate called on execution
	FEventDelegate EventDelegate;

	// Start the event timer
	template< class UserClass >
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Start(int Length, UserClass* InObj, typename FEventDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InEventMethod);

	// Pause the event timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Pause();

	// Resume the event timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Resume();

	//Reset the event and start it.
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Restart(int Length);

	//Called when the event is done.
	UFUNCTION()
	void OnExecute();

	// HELPERS
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Timer")
	bool const IsRunning();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Timer")
	bool const IsPaused();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Timer")
	float const GetLength();

private:
	void StartInternal(int Length);
};
