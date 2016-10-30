// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "MissionsComponent.h"
#include "AtomVolume.h"

AAtomVolume::AAtomVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	Color = FColor::Blue;
}


void AAtomVolume::EntityEnter(AEntity* Entity)
{
	UMissionsComponent* Missions = Cast<UMissionsComponent>(Entity->GetComponentByClass(UMissionsComponent::StaticClass()));

	if (Missions && Atom) {
		Missions->RegisterAtom(Atom);
	}
	Super::EntityEnter(Entity);
}



