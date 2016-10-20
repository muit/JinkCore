// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Entity.h"
#include "Basic_Con.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AEntity::AEntity()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MaxLive = 100;
	Live = MaxLive;
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

bool AEntity::IsAlive() const
{
	return Live > 0;
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
		UE_LOG(JinkCore, Warning, TEXT("JinkCore: AEntity::IsHostileTo tried to compare a Null Entity"));
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

	JustDied(InstigatedBy, Killer);
	JustDiedDelegate.Broadcast(InstigatedBy, Cast<AEntity>(Killer));
	if (IsPlayerControlled()) {
	}
	else if (ABasic_Con* AI = GetAI()) {
		AI->JustDied_Internal(InstigatedBy, Killer);
	}
}

ASpell* AEntity::CastSpell(TSubclassOf<ASpell> SpellType, AEntity* Target, FVector Location, FRotator Rotation, float Damage)
{
	ASpell* Spell = Cast<ASpell>(GetWorld()->SpawnActor(SpellType, &Location, &Rotation));
	if (Spell) {
		Spell->Cast(this, Target, Damage);
		return Spell;
	}
	return NULL;
}

ASpell * AEntity::CastSpellAtCaster(TSubclassOf<ASpell> SpellType, AEntity * Target, float Damage)
{
	return CastSpell(SpellType, Target, this->GetActorLocation(), this->GetActorRotation(), Damage);
}

void AEntity::ReceiveDamage_Implementation(AActor * DamagedActor, float Damage, const class UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (!IsAlive())
		return;

	Live = FMath::Clamp(Live - Damage, 0.0f, MaxLive);

	if (!IsAlive()) {
		JustDied(InstigatedBy, Cast<AEntity>(DamageCauser));
		JustDiedDelegate.Broadcast(InstigatedBy, Cast<AEntity>(DamageCauser));
		if (IsPlayerControlled()) {
		}
		else if (ABasic_Con* AI = GetAI()) {
			AI->JustDied_Internal(InstigatedBy, Cast<AEntity>(DamageCauser));
		}
	}
}

void AEntity::DoMeleAttack_Implementation(AEntity* Target)
{
	UE_LOG(JinkCore, Log, TEXT("JinkCore: %s attacked but default behaviour is been called."), *this->GetName());
}

bool AEntity::ApplyRadialDamage(float Damage, const FVector & Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor * DamageCauser, bool bDoFullDamage, ECollisionChannel DamagePreventionChannel)
{
	if (DamageCauser == NULL)
		DamageCauser = this;
	const bool checkSuccess = ApplyRadialDamageCheck(Damage, Origin, DamageRadius, DamageTypeClass, IgnoreActors, DamageCauser, bDoFullDamage, DamagePreventionChannel);
	if (checkSuccess) {
		return UGameplayStatics::ApplyRadialDamage(this, Damage, Origin, DamageRadius, DamageTypeClass, IgnoreActors, DamageCauser, GetController(), bDoFullDamage, DamagePreventionChannel);
	}
	return false;
}

bool AEntity::ApplyRadialDamageCheck_Implementation(float Damage, const FVector & Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor * DamageCauser, bool bDoFullDamage, ECollisionChannel DamagePreventionChannel)
{
	return true;
}


bool AEntity::ApplyRadialDamageWithFalloff(float Damage, float MinimumDamage, const FVector & Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor * DamageCauser, ECollisionChannel DamagePreventionChannel)
{
	if (DamageCauser == NULL)
		DamageCauser = this;

	const bool checkSuccess = ApplyRadialDamageWithFalloffCheck(Damage, MinimumDamage, Origin, DamageInnerRadius, DamageOuterRadius, DamageFalloff, DamageTypeClass, IgnoreActors, DamageCauser, DamagePreventionChannel);
	if (checkSuccess) {
		return UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, MinimumDamage, Origin, DamageInnerRadius, DamageOuterRadius, DamageFalloff, DamageTypeClass, IgnoreActors, DamageCauser, GetController(), DamagePreventionChannel);
	}
	return false;
}

bool AEntity::ApplyRadialDamageWithFalloffCheck_Implementation(float Damage, float MinimumDamage, const FVector & Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor * DamageCauser, ECollisionChannel DamagePreventionChannel)
{
	return true;
}


void AEntity::ApplyPointDamage(AActor * DamagedActor, float Damage, const FVector & HitFromDirection, const FHitResult & HitInfo, TSubclassOf<class UDamageType> DamageTypeClass, AActor * DamageCauser)
{
	if (DamageCauser == NULL)
		DamageCauser = this;

	const bool checkSuccess = ApplyPointDamageCheck(DamagedActor, Damage, HitFromDirection, HitInfo, DamageTypeClass, DamageCauser);
	if (checkSuccess) {
		UGameplayStatics::ApplyPointDamage(DamagedActor, Damage, HitFromDirection, HitInfo, GetController(), DamageCauser, DamageTypeClass);
	}
}

bool AEntity::ApplyPointDamageCheck_Implementation(AActor * DamagedActor, float Damage, const FVector & HitFromDirection, const FHitResult & HitInfo, TSubclassOf<class UDamageType> DamageTypeClass, AActor * DamageCauser)
{
	return true;
}


void AEntity::ApplyDamage(AActor * DamagedActor, float Damage, TSubclassOf<class UDamageType> DamageTypeClass, AActor * DamageCauser)
{
	if (DamageCauser == NULL)
		DamageCauser = this;

	const bool checkSuccess = ApplyDamageCheck(DamagedActor, Damage, DamageTypeClass, DamageCauser);
	if (checkSuccess) {
		UGameplayStatics::ApplyDamage(DamagedActor, Damage, GetController(), DamageCauser, DamageTypeClass);
	}
}

bool AEntity::ApplyDamageCheck_Implementation(AActor * DamagedActor, float Damage, TSubclassOf<class UDamageType> DamageTypeClass, AActor * DamageCauser)
{
	return true;
}
