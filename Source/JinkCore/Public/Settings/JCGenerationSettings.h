// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "Lenguajes/LenguajeData.h"
#include "LIAnchorType.h"
#include "LIAnchorTypeInfo.h"
#include "JCGenerationSettings.generated.h"

/**
* Find Custom Config documentation here: wiki.unrealengine.com/CustomSettings
*/
UCLASS(config = Game, defaultconfig)
class JINKCORE_API UJCGenerationSettings : public UObject
{
    GENERATED_BODY()

public:
    UJCGenerationSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
        //Add Default Anchor
        AnchorTypes.Add(ANCHOR_None, FLIAnchorTypeInfo());
    }

    UPROPERTY(config, EditAnywhere, Category = Custom)
    TMap<FString, FLIAnchorTypeInfo> AnchorTypes;
};
