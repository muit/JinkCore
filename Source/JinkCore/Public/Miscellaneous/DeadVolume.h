// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "EntityVolume.h"
#include "DeadVolume.generated.h"

/**
*
*/

UCLASS()
class JINKCORE_API ADeadVolume : public AEntityVolume
{
    GENERATED_UCLASS_BODY()

protected:
    UFUNCTION()
    virtual void EntityEnter(AEntity* Entity) override;
};
