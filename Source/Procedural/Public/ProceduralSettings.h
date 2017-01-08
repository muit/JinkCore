// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "ProceduralSettings.generated.h"

/**
 * Find Custom Config documentation here: wiki.unrealengine.com/CustomSettings 
 */
UCLASS(config = Game, defaultconfig)
class JINKCORE_API UProceduralSettings : public UObject
{
    GENERATED_BODY()
    
public:
    UProceduralSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

};
