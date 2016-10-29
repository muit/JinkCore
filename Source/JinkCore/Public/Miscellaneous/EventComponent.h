// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "EventComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEventExecuteSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JINKCORE_API UEventComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEventComponent();
	
	//The default length in seconds that will be used for the timer.
	UPROPERTY(EditAnywhere, meta = (DisplayName = "EntityExit"), Category = "Timer")
	float DefaultLength;

	// Handle to manage the timer
	FTimerHandle TimerHandle;


	// Start the event timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Start(int Length = -1);

	// Pause the event timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Pause();

	// Resume the event timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Resume();

	//Reset the event and start it.
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Restart(int Length = -1);

	UFUNCTION()
	void OnExecute();

	// HELPERS
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Timer")
	bool IsRunning();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Timer")
	bool IsPaused();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Timer")
	float GetLength();

	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FEventExecuteSignature Execute;
};
