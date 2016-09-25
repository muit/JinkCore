// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "Entity.generated.h"

UCLASS(config = Game, BlueprintType, meta = (ShortTooltip = "An entity is a type of Character that includes the basic alive features."))
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn|Entity")
	float live;


public:
	UFUNCTION(BlueprintCallable, Category = "Pawn|Entity")
	virtual bool IsAlive() const;
};
