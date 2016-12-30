// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Entity.h"

#include "Item.h"
#include "Basic_Con.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AEntity::AEntity()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    MaxLive = 100;
    Live = MaxLive;
    Damage = 10;
    FireRate = 0.7f;

    Faction = FFaction();

    MovementState = EMovementState::MS_Walk;
    WalkSpeed = 250;
    RunSpeed = 400;

    CharacterMovement = GetCharacterMovement();

    UpdateMovementSpeed();
}

// Called when the game starts or when spawned
void AEntity::BeginPlay()
{
    Super::BeginPlay();
    OnTakeAnyDamage.AddDynamic(this, &AEntity::ReceiveDamage);
}

// Called every frame
void AEntity::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );

}



/**
 * Begin ATTRIBUTES
 */

float AEntity::GetDamage() const
{
    float ModDamage = Damage;
    for (auto& ItemType : Items) {
        if(UItem* Item = UItem::GetObject(ItemType)) {
            ModDamage += Item->DamageIncrement;
        }
    }
    return ModDamage;
}

float AEntity::GetFireRate() const
{
    float ModFireRate = FireRate;
    for (auto& ItemType : Items) {
        if (UItem* Item = UItem::GetObject(ItemType)) {
            ModFireRate *= Item->FireRateCof;
        }
    }
    return ModFireRate;
}
/** End ATTRIBUTES*/

/**
* Begin ITEMS
*/
int32 AEntity::AddItem(TSubclassOf<UItem> Type)
{
    if (UItem* Item = UItem::GetObject(Type)) {
        Item->ApplyEntityModifications(this);
        return Items.Add(Type);
    }
    return 0;
}
void AEntity::RemoveItem(TSubclassOf<UItem> Type)
{
    if (Type && Items.Contains(Type)) {
        if (UItem* Item = UItem::GetObject(Type)) {
            Item->UndoEntityModifications(this);
        }
    }
    Items.RemoveSingle(Type);
}
void AEntity::RemoveItemById(int32 Id)
{
    if (Items.IsValidIndex(Id)) {
        if (UItem* Item = UItem::GetObject(Items[Id])) {
            Item->UndoEntityModifications(this);
        }
    }

    Items.RemoveAt(Id);
}
void AEntity::RemoveAllItems(TSubclassOf<UItem> Type)
{
    int32 RemovedAmount = Items.Remove(Type);
    for (int32 I = 0; I < RemovedAmount; I++) {
        if (UItem* Item = UItem::GetObject(Type)) {
            Item->UndoEntityModifications(this);
        }
    }
}
void AEntity::ClearItems()
{
    for (auto& ItemType : Items) {
        if (UItem* Item = UItem::GetObject(ItemType)) {
            Item->UndoEntityModifications(this);
        }
    }
    Items.Empty();
}
/** End ITEMS*/



bool AEntity::IsAlive() const
{
    return Live > 0;
}

bool AEntity::LiveIsUnderPercent(float Percent) const
{
    return Live/MaxLive < Percent/100;
}

void AEntity::Walk()
{
    MovementState = EMovementState::MS_Walk;
    UpdateMovementSpeed();
}

void AEntity::Run()
{
    MovementState = EMovementState::MS_Run;
    UpdateMovementSpeed();
}

void AEntity::UpdateMovementSpeed()
{
    if (CharacterMovement) {
        switch (MovementState) {
        case EMovementState::MS_None:
            CharacterMovement->MaxWalkSpeed = 0;
            break;
        case EMovementState::MS_Walk:
            CharacterMovement->MaxWalkSpeed = WalkSpeed;
            break;
        case EMovementState::MS_Run:
            CharacterMovement->MaxWalkSpeed = RunSpeed;
            break;
        default:
            //Don't change anything
            break;
        }
    }
}

void AEntity::RotateTowards(FRotator Rotation)
{
    CharacterMovement->bUseControllerDesiredRotation = true;
    if (Controller) {
        Controller->SetControlRotation(Rotation);
    }
}

void AEntity::RotateTowardsActor(AActor * Actor)
{
    if (Actor == nullptr)
        return;

    const FVector Direction = Actor->GetActorLocation() - this->GetActorLocation();
    RotateTowards(Direction.ToOrientationRotator());
}

bool AEntity::IsHostileTo(AEntity* Other) {
    if (Other == nullptr) {
        UE_LOG(LogJinkCore, Warning, TEXT("JinkCore: AEntity::IsHostileTo tried to compare a Null Entity"));
        return false;
    }
    return Faction.IsHostileTo(Other->Faction);
}

bool AEntity::IsHostileToFaction(FFaction Other) {
    return Faction.IsHostileTo(Other);
}

void AEntity::Die(AController * InstigatedBy, AEntity * Killer)
{
    if (!IsAlive())
        return;

    Live = 0;

    JustDied_Internal(InstigatedBy, Killer);
}

ASpell* AEntity::CastSpell(TSubclassOf<ASpell> SpellType, AEntity* Target, FVector Location, FRotator Rotation, float _Damage)
{
    ASpell* Spell = Cast<ASpell>(GetWorld()->SpawnActor(SpellType, &Location, &Rotation));
    if (Spell) {
        Spell->Cast(this, Target, _Damage);
        return Spell;
    }
    return NULL;
}

ASpell * AEntity::CastSpellAtCaster(TSubclassOf<ASpell> SpellType, AEntity * Target, float _Damage)
{
    return CastSpell(SpellType, Target, this->GetActorLocation(), this->GetActorRotation(), _Damage);
}

void AEntity::ReceiveDamage_Implementation(AActor * DamagedActor, float _Damage, const class UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
    if (!IsAlive())
        return;

    if (AEntity* Causer = Cast<AEntity>(DamageCauser)) {
        if (!Causer->CheckApplyAnyDamage(DamagedActor, _Damage, DamageType, DamageCauser)) {
            return;
        }
    }

    if (!CheckReceiveDamage(DamagedActor, _Damage, DamageType, DamageCauser))
        return;

    Live = FMath::Clamp(Live - _Damage, 0.0f, MaxLive);

    if (!IsAlive()) {
        AEntity* Killer = nullptr;
        if (InstigatedBy != nullptr) {
            //If the controller is valid the killer is the controlled entity
            Killer = Cast<AEntity>(InstigatedBy->GetPawn());
        }
        if(!Killer) {
            //If theres no killer, asume it is the damagecauser actor.
            Killer = Cast<AEntity>(DamageCauser);
        }

        JustDied_Internal(InstigatedBy, Killer);
    }
}

void AEntity::DoMeleAttack_Implementation(AEntity* Target)
{
    UE_LOG(LogJinkCore, Log, TEXT("JinkCore: %s attacked but default behaviour is been called."), *this->GetName());
}

bool AEntity::ApplyRadialDamage(float _Damage, const FVector & Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor * DamageCauser, bool bDoFullDamage, ECollisionChannel DamagePreventionChannel)
{
    if (!DamageCauser)
        DamageCauser = this;

    const bool checkSuccess = CheckApplyRadialDamage(_Damage, Origin, DamageRadius, DamageTypeClass, IgnoreActors, DamageCauser, bDoFullDamage, DamagePreventionChannel);
    if (checkSuccess) {
        return UGameplayStatics::ApplyRadialDamage(this, _Damage, Origin, DamageRadius, DamageTypeClass, IgnoreActors, DamageCauser, GetController(), bDoFullDamage, DamagePreventionChannel);
    }
    return false;
}

bool AEntity::ApplyRadialDamageWithFalloff(float _Damage, float MinimumDamage, const FVector & Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor * DamageCauser, ECollisionChannel DamagePreventionChannel)
{
    if (!DamageCauser)
        DamageCauser = this;

    const bool checkSuccess = CheckApplyRadialDamageWithFalloff(_Damage, MinimumDamage, Origin, DamageInnerRadius, DamageOuterRadius, DamageFalloff, DamageTypeClass, IgnoreActors, DamageCauser, DamagePreventionChannel);
    if (checkSuccess) {
        return UGameplayStatics::ApplyRadialDamageWithFalloff(this, _Damage, MinimumDamage, Origin, DamageInnerRadius, DamageOuterRadius, DamageFalloff, DamageTypeClass, IgnoreActors, DamageCauser, GetController(), DamagePreventionChannel);
    }
    return false;
}

void AEntity::ApplyPointDamage(AActor * DamagedActor, float _Damage, const FVector & HitFromDirection, const FHitResult & HitInfo, TSubclassOf<class UDamageType> DamageTypeClass, AActor * DamageCauser)
{
    if (!DamagedActor)
        return;

    if (!DamageCauser)
        DamageCauser = this;

    const bool checkSuccess = CheckApplyPointDamage(DamagedActor, _Damage, HitFromDirection, HitInfo, DamageTypeClass, DamageCauser);
    if (checkSuccess) {
        UGameplayStatics::ApplyPointDamage(DamagedActor, _Damage, HitFromDirection, HitInfo, GetController(), DamageCauser, DamageTypeClass);
    }
}

void AEntity::ApplyDamage(AActor * DamagedActor, float _Damage, TSubclassOf<class UDamageType> DamageTypeClass, AActor * DamageCauser)
{
    if (!DamagedActor)
        return;

    if (!DamageCauser)
        DamageCauser = this;

    const bool checkSuccess = CheckApplyDamage(DamagedActor, _Damage, DamageTypeClass, DamageCauser);
    if (checkSuccess) {
        UGameplayStatics::ApplyDamage(DamagedActor, _Damage, GetController(), DamageCauser, DamageTypeClass);
    }
}

//APPLY CHECKS
bool AEntity::CheckApplyRadialDamage_Implementation(float _Damage, const FVector & Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor * DamageCauser, bool bDoFullDamage, ECollisionChannel DamagePreventionChannel)
{ return true; }

bool AEntity::CheckApplyRadialDamageWithFalloff_Implementation(float _Damage, float MinimumDamage, const FVector & Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor * DamageCauser, ECollisionChannel DamagePreventionChannel)
{ return true; }

bool AEntity::CheckApplyPointDamage_Implementation(AActor * DamagedActor, float _Damage, const FVector & HitFromDirection, const FHitResult & HitInfo, TSubclassOf<class UDamageType> DamageTypeClass, AActor * DamageCauser)
{ return true; }

bool AEntity::CheckApplyDamage_Implementation(AActor * DamagedActor, float _Damage, TSubclassOf<class UDamageType> DamageTypeClass, AActor * DamageCauser)
{ return true; }

bool AEntity::CheckApplyAnyDamage_Implementation(AActor * DamagedActor, float _Damage, const class UDamageType * DamageType, AActor * DamageCauser)
{ return true; }


//RECEIVE CHECKS
bool AEntity::CheckReceiveDamage_Implementation(AActor * DamagedActor, float _Damage, const class UDamageType * DamageType, AActor * DamageCauser)
{ return true; }


void AEntity::JustDied_Internal(AController * InstigatedBy, AEntity * Killer)
{
    for (auto& ItemType : Items) {
        if (UItem* Item = UItem::GetObject(ItemType)) {
            Item->HolderJustDied(this, InstigatedBy, Killer);
        }
    }

    JustDied(InstigatedBy, Killer);
    JustDiedDelegate.Broadcast(InstigatedBy, Killer);
    if (IsPlayerControlled()) {
    }
    else if (ABasic_Con* AI = GetAI()) {
        AI->JustDied_Internal(InstigatedBy, Killer);
    }
}
