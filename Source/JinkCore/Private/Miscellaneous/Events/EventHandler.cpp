// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "EventHandler.h"

FEventHandler::FEventHandler(UObject* _Outer, int _Id) : Id(_Id) {
    bValid = true;
    bActivated = false;
    bPaused = false;
    Outer = _Outer;
    Length = 1;
}

template< class UserClass >
void FEventHandler::Bind(UserClass* Context, typename FEventDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InEventMethod) {
    //Bind Callback
    EventDelegate = FEventDelegate::CreateUObject(Context, InEventMethod);
}

bool FEventHandler::Start(float _Length)
{
    return StartInternal(_Length);
}

bool FEventHandler::StartInternal(float _Length)
{
    if (IsRunning() || _Length < 0) {
        return false;
    }

    bActivated = true;
    bPaused = false;

    Timer = new FTimer();
    Length = _Length;

    return true;
}


void FEventHandler::Pause()
{
    if (!IsRunning())
        return;

    bPaused = true;
}

void FEventHandler::Resume()
{
    if (!IsPaused()) {
        UE_LOG(LogJinkCore, Warning, TEXT("JinkCore: Tried to Resume an event, but it was not paused."));
        return;
    }

    bPaused = false;
}

void FEventHandler::Restart(float _Length)
{
    if (!EventDelegate.IsBound()) {
        UE_LOG(LogJinkCore, Warning, TEXT("JinkCore: Tried to Restart an event that is not bounded."));
        return;
    }

    if (_Length < 0) {
        _Length = Length;
    }

    //Reset the Event
    Reset();

    //Start the event again
    StartInternal(_Length);
}

void FEventHandler::Reset()
{
    //Clear the Timer
    Timer = nullptr;
    bActivated = false;
    bPaused = false;
}

void FEventHandler::Tick(float DeltaTime) {
    if (IsRunning() && !IsPaused()) {
        Timer->Tick(DeltaTime);

        //Check if Event is over
        if (Timer->GetTickCount() > Length) {
            OnExecute();
        }
    }
}

void FEventHandler::OnExecute()
{
    Reset();

    if (EventDelegate.IsBound()) {
        EventDelegate.Execute(Id);
    }
}
