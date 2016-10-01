// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Basic_Con.h"


// Sets default values
ABasic_Con::ABasic_Con()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bAttachToPawn = true;
}

// Called when the game starts or when spawned
void ABasic_Con::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABasic_Con::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABasic_Con::AttackStart(AEntity * Victim)
{
	if (!GetMe()) {
		UE_LOG(JinkCore, Warning, TEXT("JinkCore: Me is NULL"));
		return false;
	}
		
	if (!IsInCombat()) {
		if (IsValidTarget(Victim) && Victim != Target) {
			//Call Before Combat Start and use it as a validation
			if (BeforeEnterCombat(Victim)) {
				//Set Target
				Target = Victim;
				UE_LOG(JinkCore, Log, TEXT("JinkCore AI: %s started combat with %s"), *GetMe()->GetName(), *Victim->GetName());
				EnterCombat(Target);
				return true;
			}
		}
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