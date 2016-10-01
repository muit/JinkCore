// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "Faction.h"
#include "Entity.generated.h"

//~~~~~ Type Declarations ~~~~~
typedef class ABasic_Con;


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
	/**
	 * PROPERTIES
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity")
	float Live;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	FFaction Faction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity|Movement")
	EMovementState MovementState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement")
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement")
	float RunSpeed;


	//Set Movement to Walk
	UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
	virtual void Walk();
	//Set Movement to Run
	UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
	virtual void Run();
	//Update Movement Speed to the movement State
	UFUNCTION(BlueprintCallable, Category = "Entity|Movement")
	virtual void UpdateMovementSpeed();


	UFUNCTION(BlueprintCallable, Category = "Entity")
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
	bool IsHostileTo(AEntity* Other);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
	bool IsHostileToFaction(FFaction Other);

	/**
	 * HANDLERS
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity")
	ABasic_Con* GetAI() const { return Cast<ABasic_Con>(GetController()); }
};
