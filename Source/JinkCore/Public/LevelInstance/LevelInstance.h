// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "LevelInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class JINKCORE_API ULevelInstance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Instance")
	TAssetPtr<UWorld> InstancedLevel;

	ULevelInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Level Instance")
	void SetupBounds();


	//~ Begin Level Instance Interface
private:
	UPROPERTY()
	ULevelStreamingKismet* StreamingLevel;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Instance")
    FBox Bounds;

	static int32 InstanceIdCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance|Settings")
	bool bShouldBeLoaded;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance|Settings")
	bool bShouldBeVisible;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance|Settings")
	bool bShouldBlockOnLoad;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance|Settings")
	bool bInitiallyLoaded;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance|Settings")
	bool bInitiallyVisible;
	//~ End Level Instance Interface
};
