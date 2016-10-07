// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Entity.h"
#include "Basic_Con.h"


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
	UCharacterMovementComponent* _CharacterMovement = GetCharacterMovement();
	if (_CharacterMovement) {
		switch (MovementState) {
		case EMovementState::MS_None:
			_CharacterMovement->MaxWalkSpeed = 0;
			break;
		case EMovementState::MS_Walk:
			_CharacterMovement->MaxWalkSpeed = WalkSpeed;
			break;
		case EMovementState::MS_Run:
			_CharacterMovement->MaxWalkSpeed = RunSpeed;
			break;
		default:
			//Don't change anything
			break;
		}
	}
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
	if (IsPlayerControlled()) {
	}
	else if (ABasic_Con* AI = GetAI()) {
		AI->JustDied(InstigatedBy, Killer);
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
		if (IsPlayerControlled()) {
		}
		else if (ABasic_Con* AI = GetAI()) {
			AI->JustDied_Internal(InstigatedBy, Cast<AEntity>(DamageCauser));
		}
	}
}



