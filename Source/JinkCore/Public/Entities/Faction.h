// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "Faction.generated.h"

/**
 * 
 */
 /** Replicated data when playing a root motion montage. */
USTRUCT()
struct FFaction
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	FString Name;

};
