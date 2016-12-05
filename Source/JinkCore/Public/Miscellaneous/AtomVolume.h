// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Miscellaneous/CustomVolume.h"
#include "MissionAtom.h"
#include "AtomVolume.generated.h"

/**
 * 
 */
UCLASS()
class JINKCORE_API AAtomVolume : public ACustomVolume
{
	GENERATED_UCLASS_BODY()
	
public:
	// PROPERTIES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	TAssetPtr<UMissionAtom> Atom;

protected:
	UFUNCTION()
	virtual void EntityEnter(AEntity* Entity) override;
};
