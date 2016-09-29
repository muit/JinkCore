// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "Faction.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct JINKCORE_API FFaction
{
	GENERATED_USTRUCT_BODY()

	FFaction() : Name("") 
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	FString Name;
};
