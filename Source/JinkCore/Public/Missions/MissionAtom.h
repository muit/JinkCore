// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "MissionAtom.generated.h"

/**
 * 
 */
UCLASS()
class JINKCORE_API UMissionAtom : public UDataAsset
{
    GENERATED_BODY()
    
    UMissionAtom()
        : Name("")
        , Amount(1)
    {}

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    uint8 Amount;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    //TAssetPtr<Requ> AchievementIcon;
};