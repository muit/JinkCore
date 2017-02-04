// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Environment/MultiplePlaceObject.h"
#include "Components/ChildActorComponent.h"
#include "MultiplePlaceActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROCEDURAL_API AMultiplePlaceActor : public AMultiplePlaceObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (DisplayName = "Actor"))
    TSubclassOf<AActor> ActorClass;

#if WITH_EDITORONLY_DATA
    UPROPERTY()
    TArray<UChildActorComponent*> PreviewComponents;
#endif

    // Sets default values for this character's properties
    AMultiplePlaceActor();

protected:
#if WITH_EDITORONLY_DATA
    virtual void SetupPreview(const FTransform Transform, int SplinePoint);
    virtual void ClearPreviews();
#endif

    virtual void SetupFirstObject(const FTransform Transform);
    virtual void SetupOtherObject(const FTransform Transform, int Id);
};
