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
	ABasic_Con(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when an actor is possessed
	virtual void Possess(APawn* InPawn) override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	AEntity* Me;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
	AEntity* GetMe() { 
		if (!Me) Me = Cast<AEntity>(GetPawn());
		return Me;
	}

	/**
	* COMBAT
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Follow")
	bool bFollows;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Follow")
	float MinMeleDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Follow")
	float MaxMeleDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	AEntity* Target;



	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool AttackStart(AEntity* Victim);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool SetTarget(AEntity* Victim);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
	bool IsValidTarget(AEntity* Entity);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
	bool IsInCombat() const { return Target != nullptr; }

	//Combat Events
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	bool BeforeEnterCombat(AEntity* _Target);
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void EnterCombat(AEntity* _Target);
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void TargetChanged(AEntity* NewTarget);

	void JustDied_Internal(AController * InstigatedBy, AEntity* Killer);
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void JustDied(AController * InstigatedBy, AEntity* Killer);


	/**
	 * UTIL
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Movement")
	bool IsAIMoving() const {
		return GetMoveStatus() == EPathFollowingStatus::Moving;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math")
	float GetDistanceToTarget() const {
		return (Target != nullptr) ? Me->GetDistanceTo(Target) : -1.0f;
	}
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math")
	float GetSquaredDistanceToTarget() const {
		return (Target != nullptr) ? Me->GetSquaredDistanceTo(Target) : -1.0f;
	}
};
