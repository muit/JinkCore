// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Spell.h"
#include "Entity.h"
#include "SpellRules.h"


// Sets default values
ASpell::ASpell()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    _Damage = 20;
    Activated = false;
    DamageTypeClass = UDamageType::StaticClass();
    bDestroyOnCasterDead = true;
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

    //Bind caster dead
    _Caster->JustDiedDelegate.AddDynamic(this, &ASpell::Internal_OnCasterDead);

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

    if (_Caster) {
        _Caster->ApplyDamage(OtherTarget, Damage, DamageTypeClass, this);
    } else { //Apply generic Damage if there's no caster
        UGameplayStatics::ApplyDamage(OtherTarget, Damage, NULL, this, DamageTypeClass);
    }
}

void ASpell::Internal_OnCasterDead(AController* InstigatedBy, AEntity* Killer) {
    OnCasterDead(_Caster, InstigatedBy, Killer);

    if (bDestroyOnCasterDead) {
        Destroy();
    }
}

