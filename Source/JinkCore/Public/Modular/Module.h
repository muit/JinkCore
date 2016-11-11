// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Module.generated.h"

UCLASS()
class JINKCORE_API AModule : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AModule();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelInstance")
	TAssetPtr<UWorld> LevelAsset;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
