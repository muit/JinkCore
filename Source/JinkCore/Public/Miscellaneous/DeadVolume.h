// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Miscellaneous/CustomVolume.h"
#include "DeadVolume.generated.h"

/**
 * 
 */
UCLASS()
class JINKCORE_API ADeadVolume : public ACustomVolume
{
    GENERATED_UCLASS_BODY()

protected:
    UFUNCTION()
    virtual void EntityEnter(AEntity* Entity) override;
};
