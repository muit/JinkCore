// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "EventsMapComponent.h"


// Sets default values for this component's properties
UEventsMapComponent::UEventsMapComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DefaultLength = 1;
}

void UEventsMapComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    for (auto& Elem : Events)
    {
        FEventHandler Event = Elem.Value;
        if (Event.IsValid()) {
            Event.Tick(DeltaTime);
        }
    }
}

void UEventsMapComponent::Start(int Id, float Length)
{
	if (Length < 0) {
		Length = DefaultLength;
	}

	if (!Events.Contains(Id) || !Events[Id].IsValid()){
		// Create and setup event
		FEventHandler Event = FEventHandler(this, Id);
		Event.Bind<UEventsMapComponent>(this, &UEventsMapComponent::OnExecute);

		// Add the event to the map and start it
		Events.Add(Id, Event);
		Event.Start(Length);
	}
}

void UEventsMapComponent::Pause(int Id)
{
    FEventHandler Event = Events[Id];
    if (!Event.IsValid())
        return;

	Event.Pause();
}

void UEventsMapComponent::Resume(int Id)
{
    FEventHandler Event = Events[Id];
    if (!Event.IsValid())
        return;

	Event.Resume();
}

void UEventsMapComponent::Restart(int Id, float Length)
{
	FEventHandler Event = Events[Id];
	if (!Event.IsValid())
		return;

	Event.Restart(Length);
}

void UEventsMapComponent::Reset(int Id)
{
	FEventHandler Event = Events[Id];
	if (!Event.IsValid())
		return;

	Event.Reset();
}

void UEventsMapComponent::OnExecute(int Id)
{
	Execute.Broadcast(Id);
}

bool UEventsMapComponent::IsRunning(int Id) {
	FEventHandler Event = Events[Id];
	if (!Event.IsValid())
		return false;
	return Event.IsRunning();
}

bool UEventsMapComponent::IsPaused(int Id)
{
	FEventHandler Event = Events[Id];
	if (!Event.IsValid()) 
		return false;
	return Event.IsPaused();
}

float UEventsMapComponent::GetLength(int Id)
{
	FEventHandler Event = Events[Id];
	if (!Event.IsValid())
		return -1;
	return Event.GetLength();
}
