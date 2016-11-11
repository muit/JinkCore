// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Miscellaneous/CustomVolume.h"
#include "LevelInstance.generated.h"

UCLASS(meta = (ShortTooltip = "A level instance is an actor that loads an streaming level in runtime."))
class JINKCORE_API ALevelInstance : public ACustomVolume
{
	GENERATED_UCLASS_BODY()
	
public:
	//PROPERTIES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelInstance")
	TAssetPtr<UWorld> LevelAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelInstance|Loading")
	bool bShouldBeLoaded;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelInstance|Loading")
	bool bShouldBeVisible;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelInstance|Loading")
	bool bShouldBlockOnLoad;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelInstance|Loading")
	bool bInitiallyLoaded;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelInstance|Loading")
	bool bInitiallyVisible;


	UPROPERTY()
	ULevelStreamingKismet* AssignedLevel;
	UPROPERTY()
	bool bRegisteredInWorld;
	UPROPERTY(BlueprintReadOnly, Category = "LevelInstance")
	int32 InstanceIndex;



	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool LoadLevel();
	bool RegistryLevelInWorld();
	void UnloadLevel();
	
public:
	//Helpers
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LevelInstance")
	bool IsLoaded() { return AssignedLevel != nullptr; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LevelInstance")
	bool IsRegisteredInWorld() { return bRegisteredInWorld; }


	//STATIC
	static int32 InstanceCount;
};
