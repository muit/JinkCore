// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "DeadVolume.h"

void ADeadVolume::EntityEnter(AEntity * Entity)
{
    Entity->Die();
    ReceiveEntityEnter(Entity);
    OnEntityEnter.Broadcast(Entity);
}
