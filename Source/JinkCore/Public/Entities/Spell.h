// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Spell.generated.h"

//~~~~~ Type Declarations ~~~~~
typedef class AEntity;

UCLASS()
class JINKCORE_API ASpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpell();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spell")
	AEntity* _Caster;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spell")
	AEntity* _Target;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float _Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	TSubclassOf<UDamageType> DamageTypeClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spell")
	bool Activated;

	UFUNCTION(BlueprintCallable, Category = "Spell")
	void Cast(AEntity* Caster, AEntity* Target, float Damage = 0.0f);
	UFUNCTION(BlueprintCallable, Category = "Spell")
	void UpdateTarget(AEntity* Target);

	UFUNCTION(BlueprintCallable, Category = "Spell")
	void ApplyDamage(AEntity* OtherTarget = NULL, float Damage = 0);
	

	/**
	 * EVENTS
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Spell")
	void OnCast(AEntity* Caster, AEntity* Target);
	UFUNCTION(BlueprintImplementableEvent, Category = "Spell")
	void OnTargetChange(AEntity* Target);
};
