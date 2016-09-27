// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Entity.h"


// Sets default values
AEntity::AEntity()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Live = 100;
	
	MovementState = EMovementState::MS_Walk;
	WalkSpeed = 250;
	RunSpeed = 400;

	UpdateMovementSpeed();
}

// Called when the game starts or when spawned
void AEntity::BeginPlay()
{
	Super::BeginPlay();
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


