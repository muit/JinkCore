// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "EntityVolume.h"
#include "DeadVolume.generated.h"

/**
*
*/
UCLASS(Blueprintable)
class JINKCORE_API ADeadVolume : public AEntityVolume
{
    GENERATED_BODY()

protected:

    virtual void EntityEnter(AEntity* Entity) override;
};
