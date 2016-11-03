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
	template< class UserClass >
	void Setup(UserClass* Context, typename FEventDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InEventMethod, int _Id = 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	int Id;
	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	bool bActivated;


	UWorld* World;
	// Handle to manage the timer
	FTimerHandle TimerHandle;
	//Delegate called on execution
	FEventDelegate EventDelegate;

	// Start the event timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Start(float Length);

	// Pause the event timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Pause();

	// Resume the event timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Resume();

	//Reset the event and start it.
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Restart(float Length);
	//Reset the event and start it.
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Reset();

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
