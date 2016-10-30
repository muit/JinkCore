// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "EventHandler.h"
#include "EventsMapComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventsMapExecuteSignature, int, Id);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JINKCORE_API UEventsMapComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEventsMapComponent();
	

	//The default length in seconds that will be used for the timer.
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Length"), Category = "Timer")
	float DefaultLength;

	// Handle to manage the timer
	TMap<int, UEventHandler*> Events;


	// Start the event timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Start(int Id, int Length = -1);

	// Pause the event timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Pause(int Id);

	// Resume the event timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Resume(int Id);

	//Reset the event and start it.
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void Restart(int Id, int Length = -1);

	UFUNCTION()
	void OnExecute(int Id);

	// HELPERS
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Timer")
	bool IsRunning(int Id);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Timer")
	bool IsPaused(int Id);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Timer")
	float GetLength(int Id);


	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FEventsMapExecuteSignature Execute;
};
