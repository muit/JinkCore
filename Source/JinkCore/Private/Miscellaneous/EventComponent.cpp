// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "EventComponent.h"


// Sets default values for this component's properties
UEventComponent::UEventComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	DefaultLength = 1;
}

void UEventComponent::Start(int Length)
{
	if (IsRunning())
		return;

	if (Length < 0)
	{
		Length = DefaultLength;
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UEventComponent::OnExecute, Length, false);
}

void UEventComponent::Pause()
{
	if (!TimerHandle.IsValid() || !IsRunning()) {
		return;
	}

	if (!GetWorld()->GetTimerManager().IsTimerPaused(TimerHandle)) {
		GetWorld()->GetTimerManager().PauseTimer(TimerHandle);
	}
}

void UEventComponent::Resume()
{
	if (!TimerHandle.IsValid() || !IsPaused()) {
		return;
	}

	if (GetWorld()->GetTimerManager().IsTimerPaused(TimerHandle)) {
		GetWorld()->GetTimerManager().UnPauseTimer(TimerHandle);
	}
}

void UEventComponent::Restart(int Length)
{
	//Clear the Timer
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	//Start it again
	Start(Length);
}

void UEventComponent::OnExecute()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	Execute.Broadcast();
}

bool UEventComponent::IsRunning() {
	return GetWorld()->GetTimerManager().IsTimerActive(TimerHandle);
}

bool UEventComponent::IsPaused()
{
	return GetWorld()->GetTimerManager().IsTimerPaused(TimerHandle);
}

float UEventComponent::GetLength()
{
	return 	GetWorld()->GetTimerManager().GetTimerRate(TimerHandle);
}
