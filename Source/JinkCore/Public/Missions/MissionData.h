// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "MissionAtom.h"
#include "MissionData.generated.h"

UCLASS(Blueprintable)
class JINKCORE_API UMissionData : public UDataAsset
{
    GENERATED_BODY()

public:
    UMissionData()
        : Title("")
        , Description(FText::FromString(""))
    {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Title;


    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    /*
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> MissionsRequired;
    */

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TAssetPtr<UMissionAtom>> RequiredAtoms;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TAssetPtr<UMissionAtom>> SuccessAtoms;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TAssetPtr<UMissionAtom>> FailureAtoms;
};