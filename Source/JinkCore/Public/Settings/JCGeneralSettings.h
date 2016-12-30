// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "Lenguajes/LenguajeData.h"
#include "JCGeneralSettings.generated.h"

/**
 * Find Custom Config documentation here: wiki.unrealengine.com/CustomSettings 
 */
UCLASS(config = Game, defaultconfig)
class JINKCORE_API UJCGeneralSettings : public UObject
{
    GENERATED_BODY()
    
public:
    UJCGeneralSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

    UPROPERTY(config, EditAnywhere, Category = Custom)
    TArray<FString> Factions;

    /** Lenguajes List */
    UPROPERTY(config, EditAnywhere, Category = Lenguajes)
    TArray<FLenguajeData> Lenguajes;
};
