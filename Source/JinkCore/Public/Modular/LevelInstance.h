// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "LevelInstance.generated.h"

UCLASS(config = Game, BlueprintType, meta = (ShortTooltip = "A level instance is an actor that loads an streaming level in runtime."))
class JINKCORE_API ALevelInstance : public AActor
{
	GENERATED_BODY()
	
public:
	/**
	* Default UObject constructor.
	*/
	ALevelInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//COMPONENTS
	UPROPERTY()
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelInstance")
	TAssetPtr<UWorld> LevelAsset;

	UPROPERTY(BlueprintReadWrite, Category = "LevelInstance")
	ULevelStreaming* AssignedLevel;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
	bool LoadLevelInstance();
	
	//STATIC
	static int32 InstanceIndex;
};
