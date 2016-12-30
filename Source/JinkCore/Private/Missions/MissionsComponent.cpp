// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "MissionsComponent.h"


// Sets default values for this component's properties
UMissionsComponent::UMissionsComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
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

void UMissionsComponent::RegisterAtom(TAssetPtr<UMissionAtom> Atom, uint8 Amount) {
    if (Amount <= 0)
        return;

    if (Amount == 1)
    {
        Atoms.AddUnique(Atom);

        CheckMissionStates();
        return;
    }
}

void UMissionsComponent::StartMission(TAssetPtr<UMissionData> Mission) {
    if (!Mission.IsValid())
        return;

    FMissionItem MissionItem = FMissionItem();
    MissionItem.Mission = Mission;
    MissionItem.State = EMissionState::MS_IN_PROGRESS;

    if (CompletedMissions.Contains(MissionItem))
        return;

    Missions.AddUnique(MissionItem);

    //Call Delegate
    OnMissionStarted.Broadcast(MissionItem.GetData());

    CheckMissionState(MissionItem);
}

void UMissionsComponent::CompleteMission(FMissionItem& MissionItem, bool Success) {
    if (MissionItem.State != EMissionState::MS_NOT_STARTED && Missions.Contains(MissionItem)) {
        MissionItem.State = Success ? EMissionState::MS_SUCCESS : EMissionState::MS_FAILURE;
        Missions.Remove(MissionItem);

        CompletedMissions.AddUnique(MissionItem);

        //Call Delegate
        OnMissionCompleted.Broadcast(MissionItem.State, MissionItem.GetData());
    }
}

void UMissionsComponent::CheckMissionStates()
{
    TArray<FMissionItem> SuccessList;
    TArray<FMissionItem> FailureList;
    for (auto& I : Missions)
    {
        EMissionState NewState = GetNewMissionState(I);
        if (NewState == EMissionState::MS_SUCCESS) {
            SuccessList.Add(I);
        }
        else if (NewState == EMissionState::MS_FAILURE) {
            FailureList.Add(I);
        }
    }

    for (auto& SM : SuccessList) {
        CompleteMission(SM, true);
    }

    for (auto& SM : FailureList) {
        CompleteMission(SM, false);
    }
}

void UMissionsComponent::CheckMissionState(FMissionItem& MissionItem) {
    EMissionState NewState = GetNewMissionState(MissionItem);
    if (NewState == EMissionState::MS_SUCCESS) {
        CompleteMission(MissionItem, true);
    }
    if (NewState == EMissionState::MS_FAILURE) {
        CompleteMission(MissionItem, false);
    }
}

EMissionState UMissionsComponent::GetNewMissionState(FMissionItem& MissionItem) {
    if (MissionItem.State == EMissionState::MS_IN_PROGRESS) {
        //Get Mission Info
        if (UMissionData* Data = MissionItem.GetData()) {
            //Check Failure. Skip if none.
            if (Data->FailureAtoms.Num() > 0) {
                bool HasFailed = false;
                for (auto& SF : Data->FailureAtoms) {
                    if (Atoms.Contains(SF)) {
                        HasFailed = true;
                        break;
                    }
                }
                if (HasFailed) {
                    return EMissionState::MS_FAILURE;
                }
            }

            //Check Success. Skip if none.
            if (Data->SuccessAtoms.Num() > 0) {
                bool IsCompleted = true;
                for (auto& SA : Data->SuccessAtoms) {
                    if (!Atoms.Contains(SA)) {
                        IsCompleted = false;
                        break;
                    }
                }

                if (IsCompleted) {
                    return EMissionState::MS_SUCCESS;
                }
            }
        }
    }
    return MissionItem.State;
}
