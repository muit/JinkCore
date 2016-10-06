// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Spell.h"
#include "Entity.h"


// Sets default values
ASpell::ASpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_Damage = 20;
	Activated = false;
	DamageTypeClass = UDamageType::StaticClass();;
}

// Called when the game starts or when spawned
void ASpell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpell::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ASpell::Cast(AEntity* Caster, AEntity* Target, float Damage)
{
	if (!Caster)
		return;

	if (Damage != 0)
		_Damage = Damage;

	_Caster = Caster;
	_Target = Target;
	Activated = true;

	OnCast(Caster, Target);
}

void ASpell::UpdateTarget(AEntity* Target)
{
	if (!Target)
		return;

	_Target = Target;
	OnTargetChange(Target);
}

void ASpell::ApplyDamage(AEntity* OtherTarget, float Damage)
{
	//If there's no OtherTarget, Use the main target.
	if (!OtherTarget)
		OtherTarget = _Target;
	if (!OtherTarget)
		return;

	if (Damage == 0)
		Damage = _Damage;

	UGameplayStatics::ApplyDamage(OtherTarget, Damage, _Caster->GetController(), this, DamageTypeClass);
}

