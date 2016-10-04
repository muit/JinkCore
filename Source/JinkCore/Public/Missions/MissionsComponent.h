// Copyright 2015-2016 Piperift. All Rights Reserved.

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
	MS_SUCCESS	   UMETA(DisplayName = "Success"),
	MS_FAILURE	   UMETA(DisplayName = "Failure")
};

USTRUCT(Blueprintable)
struct FMissionItem {
	GENERATED_USTRUCT_BODY()

public:

	FMissionItem(TAssetPtr<UMissionData> Mission) {
		this->Mission = Mission;
	}

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

	FORCEINLINE bool operator==(FMissionItem& Other) const {
		return Other.Mission == Mission;
	}

	FORCEINLINE bool operator!=(FMissionItem& Other) const{
		return Other.Mission != Mission;
	}
};

USTRUCT(Blueprintable)
struct FAtomItem {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Atom")
	TAssetPtr<URegisterAtom> Atom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atom")
	uint8 Amount;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMissionStarted, UMissionData*, Mission);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMissionCompleted, EMissionState, State, UMissionData*, Mission);

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
	TArray<TAssetPtr<URegisterAtom>> Atoms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	TArray<FAtomItem> AtomsWithAmount;



	UFUNCTION(BlueprintCallable, Category = "Mission")
	void RegisterAtom(TAssetPtr<URegisterAtom> Atom, uint8 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void StartMission(TAssetPtr<UMissionData> Mission);
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void CompleteMission(FMissionItem& MissionItem, bool success);


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
