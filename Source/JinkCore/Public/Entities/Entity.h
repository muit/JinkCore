// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "Entity.generated.h"

UCLASS(Blueprintable)
class JINKCORE_API AEntity : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEntity();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Entity)
	float live;
};
