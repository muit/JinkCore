// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "DeadVolume.h"

ADeadVolume::ADeadVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	bSolid = true;
	Color = FColor::Red;
}

void ADeadVolume::EntityEnter(AEntity * Entity)
{
	Entity->Die();
	Super::EntityEnter(Entity);
}
