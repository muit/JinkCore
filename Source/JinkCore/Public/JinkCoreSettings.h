// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "Lenguajes/LenguajeData.h"
#include "JinkCoreSettings.generated.h"

/**
 * Find Custom Config documentation here: wiki.unrealengine.com/CustomSettings 
 */
UCLASS(config = Game, defaultconfig)
class JINKCORE_API UJinkCoreSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UJinkCoreSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(config, EditAnywhere, Category = Custom)
	TArray<FString> Factions;

	/** Or add min, max or clamp values to the settings */
	UPROPERTY(config, EditAnywhere, Category = Custom, meta = (UIMin = 1, ClampMin = 1))
	int32 ClampedIntSetting;

	/** We can even use asset references */
	UPROPERTY(config, EditAnywhere, Category = Materials, meta = (AllowedClasses = "MaterialInterface"))
	FStringAssetReference StringMaterialAssetReference;

	/** Lenguajes List */
	UPROPERTY(config, EditAnywhere, Category = Lenguajes)
	TArray<FLenguajeData> Lenguajes;
};
