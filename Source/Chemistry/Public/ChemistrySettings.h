// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "ChemistrySettings.generated.h"

/**
 * Find Custom Config documentation here: wiki.unrealengine.com/CustomSettings 
 */
UCLASS(config = Game, defaultconfig)
class CHEMISTRY_API UChemistrySettings : public UObject
{
    GENERATED_BODY()
    
public:
    UChemistrySettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

};
