// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "EventHandler.h"

UEventHandler::UEventHandler() {
	this->Id = 0;
	bActivated = false;
}

void UEventHandler::Setup(UObject* Context, int _Id) {
	WorldContext = Context;
	Id = _Id;
}

template< class UserClass >
void UEventHandler::Start(int Length, UserClass* InObj, typename FEventDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InEventMethod)
{
	if (!WorldContext)
		return;

	if (IsRunning() || Length < 0) {
		return;
	}

	//SetupCallback
	EventDelegate = FEventDelegate::CreateUObject(InObj, InEventMethod);

	StartInternal(Length);
}


void UEventHandler::Pause()
{
	if (!WorldContext)
		return;

	if (!TimerHandle.IsValid() || !IsRunning()) {
		return;
	}

	if (!WorldContext->GetWorld()->GetTimerManager().IsTimerPaused(TimerHandle)) {
		WorldContext->GetWorld()->GetTimerManager().PauseTimer(TimerHandle);
	}
}

void UEventHandler::Resume()
{
	if (!WorldContext)
		return;

	if (!TimerHandle.IsValid() || !IsPaused()) {
		return;
	}

	if (WorldContext->GetWorld()->GetTimerManager().IsTimerPaused(TimerHandle)) {
		WorldContext->GetWorld()->GetTimerManager().UnPauseTimer(TimerHandle);
	}
}

void UEventHandler::Restart(int Length)
{
	if (!WorldContext)
		return;

	if (!EventDelegate.IsBound()) {
		UE_LOG(JinkCore, Warning, TEXT("JinkCore: Tried to Restart an event that is not bounded."));
		return;
	}

	if (Length < 0) {
		Length = GetLength();
	}

	//Clear the Timer
	WorldContext->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	//Start the event again
	StartInternal(Length);
}

void UEventHandler::Reset()
{
	//Clear the Timer
	WorldContext->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	bActivated = false;
}

void UEventHandler::OnExecute()
{
	WorldContext->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	if (EventDelegate.IsBound()) {
		EventDelegate.Execute(Id);
	}
}

bool const UEventHandler::IsRunning() {
	if (!WorldContext || !TimerHandle.IsValid()) {
		return false;
	}
	return WorldContext->GetWorld()->GetTimerManager().IsTimerActive(TimerHandle);
}

bool const UEventHandler::IsPaused()
{
	if (!WorldContext || !TimerHandle.IsValid()) {
		return false;
	}
	return WorldContext->GetWorld()->GetTimerManager().IsTimerPaused(TimerHandle);
}

float const UEventHandler::GetLength()
{
	if (!WorldContext || !TimerHandle.IsValid()) {
		return -1;
	}
	return WorldContext->GetWorld()->GetTimerManager().GetTimerRate(TimerHandle);
}


void UEventHandler::StartInternal(int Length)
{
	if (IsRunning() || Length < 0) {
		return;
	}

	WorldContext->GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UEventHandler::OnExecute, Length, false);
	bActivated = true;
}



