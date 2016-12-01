// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

UCLASS()
class JINKCORE_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
