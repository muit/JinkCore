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
	static int32 InstanceIdCount;

	UPROPERTY(BlueprintReadOnly, Category = "Level Instance")
	int32 InstanceId;

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

	UFUNCTION(BlueprintCallable, Category = "Level Instance")
	bool SpawnLevel(FTransform Transform);
	UFUNCTION(BlueprintCallable, Category = "Level Instance")
	bool LoadLevel();
	UFUNCTION(BlueprintCallable, Category = "Level Instance")
	void SetLevelVisibility(bool NewVisibility);
	UFUNCTION(BlueprintCallable, Category = "Level Instance")
	void UnloadLevel();
	UFUNCTION(BlueprintCallable, Category = "Level Instance")
	void RemoveLevel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
	FString GetUniqueName();

	//Helpers
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
	bool IsRegistered() { return StreamingLevel != nullptr; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
	bool IsLoaded() { return IsRegistered() && StreamingLevel->IsLevelLoaded(); }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
	bool IsVisible() { return IsRegistered() && StreamingLevel->IsLevelVisible(); }
	//~ End Level Instance Interface
};
