// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Basic_Con.h"


// Sets default values
ABasic_Con::ABasic_Con(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAttachToPawn = true;

	bFollows = true;
	MinMeleDistance = 100.0f;
	MaxMeleDistance = MinMeleDistance + 100.0f;
}

// Called when the game starts or when spawned
void ABasic_Con::BeginPlay()
{
	Super::BeginPlay();
}

void ABasic_Con::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	//Setup Me
	Me = GetMe();
}

// Called every frame
void ABasic_Con::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsInCombat())
		return;

	if (bFollows) {
		if (!IsAIMoving()) {
			if (GetSquaredDistanceToTarget() < FMath::Pow(MaxMeleDistance, 2))
			{
				//Mele Attack
			}
			else
			{
				//If not moving, follow the target
				MoveToActor(Target, MinMeleDistance);
			}
		}
	}
	else if(IsAIMoving()) {
		StopMovement();
	}
}

bool ABasic_Con::AttackStart(AEntity * Victim)
{
	if (!GetMe()) {
		UE_LOG(JinkCore, Warning, TEXT("JinkCore: Me is NULL, can't start combat."));
		return false;
	}
		
	if (!IsInCombat()) {
		if (IsValidTarget(Victim) && Victim != Target) {
			//Call Before Combat Start and use it as a validation
			if (BeforeEnterCombat(Victim)) {
				//Set Target
				Target = Victim;
				UE_LOG(JinkCore, Log, TEXT("JinkCore AI: %s started combat with '%s'"), *GetMe()->GetName(), *Victim->GetName());
				EnterCombat(Target);
				return true;
			}
		}
	}
	else {
		return SetTarget(Victim);
	}

	return false;
}

bool ABasic_Con::SetTarget(AEntity * Victim) {
	if (IsValidTarget(Victim) && Victim != Target) {
		Target = Victim;
		UE_LOG(JinkCore, Log, TEXT("JinkCore AI: %s changed target to '%s'"), *GetMe()->GetName(), *Victim->GetName());
		TargetChanged(Target);
		return true;
	}
	return false;
}

bool ABasic_Con::IsValidTarget(AEntity * Entity)
{
	if (Entity != nullptr) {
		return Entity != GetMe() && GetMe()->IsHostileTo(Entity);
	}
	return false;
}

bool ABasic_Con::BeforeEnterCombat_Implementation(AEntity* Entity) {
	return true;
}
