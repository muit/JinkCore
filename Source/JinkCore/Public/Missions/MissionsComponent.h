// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	TAssetPtr<UMissionData> Mission;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	EMissionState State;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JINKCORE_API UMissionsComponent : public USceneComponent
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
	TArray<TAssetPtr<URegisterAtom>> RegisteredAtoms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	TArray<FMissionItem> MissionList;
	
};
