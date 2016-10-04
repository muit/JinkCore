// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "MissionsComponent.h"


// Sets default values for this component's properties
UMissionsComponent::UMissionsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UMissionsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMissionsComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UMissionsComponent::RegisterAtom(TAssetPtr<URegisterAtom> Atom, uint8 Amount) {
	if (Amount <= 0)
		return;

	if (Amount == 1)
	{
		Atoms.AddUnique(Atom);

		//CheckMissionStates();
		return;
	}
	/*
	FAtomItem AtomItem = AtomsWithAmount.;

	if (AtomItem)
	{
		AtomItem.Atom = Atom;
		AtomItem
	}
	else {
		AtomItem.Amount += Amount;

	}*/
}

void UMissionsComponent::StartMission(TAssetPtr<UMissionData> Mission) {
	TSharedPtr<FMissionItem> MissionItem = MakeShareable(new FMissionItem(Mission));

	Missions.AddUnique(MissionItem);
}

void UMissionsComponent::CompleteMission(FMissionItem& MissionItem, bool Success) {
	if (MissionItem.State != EMissionState::MS_NOT_STARTED && Missions.Contains(MissionItem)) {
		MissionItem.State = Success ? EMissionState::MS_SUCCESS : EMissionState::MS_FAILURE;
		Missions.Remove(MissionItem);

		CompletedMissions.AddUnique(MissionItem);
	}
}
