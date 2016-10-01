// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "AIController.h"
#include "Entity.h"
#include "Basic_Con.generated.h"

/**
 * 
 */
UCLASS(config = Game, BlueprintType, meta = (ShortTooltip = "A basic AI that does nothing but gives the base for more complex ones."))
class JINKCORE_API ABasic_Con : public AAIController
{
	GENERATED_BODY()

public:
	ABasic_Con();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	AEntity* Me;
	UFUNCTION(BlueprintCallable, Category = "AI")
	AEntity* GetMe() { 
		if (!Me) Me = Cast<AEntity>(GetPawn());
		return Me;
	}

	/**
	* COMBAT
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
	AEntity* Target;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool AttackStart(AEntity* Victim);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
	bool IsValidTarget(AEntity* Entity);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
	bool IsInCombat() const { return Target != nullptr; }

	//Combat Events
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	bool BeforeEnterCombat(AEntity* Entity);
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void EnterCombat(AEntity* Entity);

};
