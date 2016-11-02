// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "EventsMapComponent.h"


// Sets default values for this component's properties
UEventsMapComponent::UEventsMapComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	DefaultLength = 1;
}

void UEventsMapComponent::Start(int Id, float Length)
{
	if (Length < 0) {
		Length = DefaultLength;
	}

	if (!Events.Contains(Id) || !Events[Id]){
		// Create and setup event
		UEventHandler* Event = NewObject<UEventHandler>();
		Event->Setup(this, Id);

		// Add the event to the map and start it
		Events.Add(Id, Event);
		Event->Start(Length, this, &UEventsMapComponent::OnExecute);
	}
}

void UEventsMapComponent::Pause(int Id)
{
	UEventHandler* Event = Events[Id];
	if (!Event)
		return;
	Event->Pause();
}

void UEventsMapComponent::Resume(int Id)
{
	UEventHandler* Event = Events[Id];
	if (!Event)
		return;
	Event->Resume();
}

void UEventsMapComponent::Restart(int Id, float Length)
{
	UEventHandler* Event = Events[Id];
	if (!Event)
		return;

	Event->Restart(Length);
}

void UEventsMapComponent::Reset(int Id)
{
	UEventHandler* Event = Events[Id];
	if (!Event)
		return;

	Event->Reset();
}

void UEventsMapComponent::OnExecute(int Id)
{
	Execute.Broadcast(Id);
}

bool UEventsMapComponent::IsRunning(int Id) {
	UEventHandler* Event = Events[Id];
	if (!Event) 
		return false;
	return Event->IsRunning();
}

bool UEventsMapComponent::IsPaused(int Id)
{
	UEventHandler* Event = Events[Id];
	if (!Event) 
		return false;
	return Event->IsPaused();
}

float UEventsMapComponent::GetLength(int Id)
{
	UEventHandler* Event = Events[Id];
	if (!Event) 
		return -1;
	return Event->GetLength();
}
