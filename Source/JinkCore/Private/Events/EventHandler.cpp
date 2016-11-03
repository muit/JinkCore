// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "EventHandler.h"

UEventHandler::UEventHandler() {
	this->Id = 0;
	bActivated = false;
}

template< class UserClass >
void UEventHandler::Setup(UserClass* Context, typename FEventDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InEventMethod, int _Id) {
	UObject* ObjContext = Cast<UObject>(Context);
	if (ObjContext) {
		World = ObjContext->GetWorld();
	}
	Id = _Id;

	//SetupCallback
	EventDelegate = FEventDelegate::CreateUObject(Context, InEventMethod);
}

void UEventHandler::Start(float Length)
{
	if (!World)
		return;

	if (IsRunning() || Length < 0) {
		return;
	}

	StartInternal(Length);
}


void UEventHandler::Pause()
{
	if (!World)
		return;

	if (!TimerHandle.IsValid() || !IsRunning()) {
		return;
	}

	if (!World->GetTimerManager().IsTimerPaused(TimerHandle)) {
		World->GetTimerManager().PauseTimer(TimerHandle);
	}
}

void UEventHandler::Resume()
{
	if (!World)
		return;

	if (!TimerHandle.IsValid() || !IsPaused()) {
		return;
	}

	if (World->GetTimerManager().IsTimerPaused(TimerHandle)) {
		World->GetTimerManager().UnPauseTimer(TimerHandle);
	}
}

void UEventHandler::Restart(float Length)
{
	if (!World)
		return;

	if (!EventDelegate.IsBound()) {
		UE_LOG(JinkCore, Warning, TEXT("JinkCore: Tried to Restart an event that is not bounded."));
		return;
	}

	if (Length < 0) {
		Length = GetLength();
	}

	//Reset the Event
	Reset();

	//Start the event again
	StartInternal(Length);
}

void UEventHandler::Reset()
{
	//Clear the Timer
	World->GetTimerManager().ClearTimer(TimerHandle);
	bActivated = false;
}

void UEventHandler::OnExecute()
{
	World->GetTimerManager().ClearTimer(TimerHandle);

	if (EventDelegate.IsBound()) {
		EventDelegate.Execute(Id);
	}
}

bool const UEventHandler::IsRunning() {
	if (!World || !TimerHandle.IsValid()) {
		return false;
	}
	//Test
	FTimerManager& TimerManager = World->GetTimerManager();
	return TimerManager.IsTimerActive(TimerHandle);
}

bool const UEventHandler::IsPaused()
{
	if (!World || !TimerHandle.IsValid()) {
		return false;
	}
	return World->GetTimerManager().IsTimerPaused(TimerHandle);
}

float const UEventHandler::GetLength()
{
	if (!World || !TimerHandle.IsValid()) {
		return -1;
	}
	return World->GetTimerManager().GetTimerRate(TimerHandle);
}


void UEventHandler::StartInternal(int Length)
{
	if (IsRunning() || Length < 0) {
		return;
	}

	World->GetTimerManager().SetTimer(TimerHandle, this, &UEventHandler::OnExecute, Length, false);
	bActivated = true;
}



