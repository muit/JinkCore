// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "MissionData.h"
#include "JinkGameMode.h"
#include "MissionsComponent.generated.h"

UENUM(BlueprintType)
enum class EMissionState: uint8
{
    MS_NOT_STARTED UMETA(DisplayName = "Not Started"),
    MS_IN_PROGRESS UMETA(DisplayName = "In Progress"),
    MS_SUCCESS       UMETA(DisplayName = "Success"),
    MS_FAILURE       UMETA(DisplayName = "Failure")
};

USTRUCT(Blueprintable)
struct FMissionItem {
    GENERATED_USTRUCT_BODY()

public:

    FMissionItem() : State(EMissionState::MS_NOT_STARTED) {}

    UPROPERTY(EditAnywhere, Category = "Mission")
    TAssetPtr<UMissionData> Mission;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    EMissionState State;


    UMissionData* GetData() {
        if (Mission.IsPending())
        {
            const FStringAssetReference& AssetRef = Mission.ToStringReference();
            return Cast<UMissionData>(Mission.ToStringReference().TryLoad());
        }
        return Mission.Get();
    }

    FORCEINLINE bool operator == (const FMissionItem &b) const {
        return b.Mission == Mission;
    }
};


USTRUCT(Blueprintable)
struct FAtomItem {
    GENERATED_USTRUCT_BODY()

public:

    UPROPERTY(EditAnywhere, Category = "Atom")
    TAssetPtr<UMissionAtom> Atom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atom")
    uint8 Amount;

    FORCEINLINE bool operator == (const FAtomItem &b) const {
        return b.Atom == Atom;
    }
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMissionStarted, TAssetPtr<UMissionData>, Mission);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMissionCompleted, EMissionState, State, TAssetPtr<UMissionData>, Mission);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JINKCORE_API UMissionsComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    // Sets default values for this component's properties
    UMissionsComponent();

    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    TArray<FMissionItem> Missions;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    TArray<FMissionItem> CompletedMissions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    TArray<TAssetPtr<UMissionAtom>> Atoms;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    TArray<FAtomItem> AtomsWithAmount;



    UFUNCTION(BlueprintCallable, Category = "Mission")
    void RegisterAtom(TAssetPtr<UMissionAtom> Atom, uint8 Amount = 1);

    UFUNCTION(BlueprintCallable, Category = "Mission")
    void StartMission(TAssetPtr<UMissionData> Mission);
    UFUNCTION(BlueprintCallable, Category = "Mission")
    void CompleteMission(FMissionItem& MissionItem, bool success);


    UFUNCTION(BlueprintCallable, Category = "Mission")
    void CheckMissionStates();

    UFUNCTION(BlueprintCallable, Category = "Mission")
    void CheckMissionState(FMissionItem& MissionItem);

    EMissionState GetNewMissionState(FMissionItem& MissionItem);

    /**
    * HANDLERS
    */
    UFUNCTION(BlueprintCallable, Category = "Mission")
    UMissionData* GetMissionData(FMissionItem MissionItem) {
        return MissionItem.GetData();
    }

    /**
     * DELEGATES
     */
    UPROPERTY(BlueprintAssignable, Category = "Mission")
    FMissionStarted OnMissionStarted;
    UPROPERTY(BlueprintAssignable, Category = "Mission")
    FMissionCompleted OnMissionCompleted;
};
