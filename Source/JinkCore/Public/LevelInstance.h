// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

//#include "Engine/Level.h"
#include "LevelInstance.generated.h"

/**
 * 
 */
UCLASS()
class JINKCORE_API ULevelInstance : public UObject
{
	GENERATED_BODY()

public:
	ULevelInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Instance")
	FBox Bounds;

};
