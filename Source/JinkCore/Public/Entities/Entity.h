// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "Entity.generated.h"

//MovementState to define different types of movements.
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	MS_None	    UMETA(DisplayName = "None"),
	MS_Walk 	UMETA(DisplayName = "Walk"),
	MS_Run  	UMETA(DisplayName = "Run")
};


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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
	float Live;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity|Movement")
	EMovementState MovementState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement")
	float RunSpeed;

public:
	UFUNCTION(BlueprintCallable, Category = "Entity")
	virtual bool IsAlive() const;

	//Set Movement to Walk
	UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
	virtual void Walk();

	//Set Movement to Run
	UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
	virtual void Run();

	UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
	virtual void UpdateMovementSpeed();

};
