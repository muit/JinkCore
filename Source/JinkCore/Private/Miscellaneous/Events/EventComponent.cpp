// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "EventComponent.h"


// Sets default values for this component's properties
UEventComponent::UEventComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DefaultLength = 1;

	EventHandler = Cast<UEventHandler>(UEventHandler::StaticClass()->GetDefaultObject());
}

void UEventComponent::BeginPlay()
{
    EventHandler->Setup(this, &UEventComponent::OnExecute);

    if (bAutoActivate) {
        Start();
    }
}

void UEventComponent::Start(float Length)
{
	if (Length < 0) {
		Length = DefaultLength;
	}
	EventHandler->Start(Length);
}

void UEventComponent::Pause()
{
	EventHandler->Pause();
}

void UEventComponent::Resume()
{
	EventHandler->Resume();
}

void UEventComponent::Restart(float Length)
{
	EventHandler->Restart(Length);
}

void UEventComponent::Reset()
{
	EventHandler->Reset();
}

void UEventComponent::OnExecute(int Id)
{
    //
    if (bLooping) {
        Start();
    }
	Execute.Broadcast();
}

bool UEventComponent::IsRunning() {
	return EventHandler->IsRunning();
}

bool UEventComponent::IsPaused()
{
	return EventHandler->IsPaused();
}

float UEventComponent::GetLength()
{
	return EventHandler->GetLength();
}
