// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Environment/MultiplePlaceObject.h"
#include "Engine/DestructibleMesh.h"
#include "PhysicsEngine/DestructibleActor.h"
#include "MultiplePlaceDMesh.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Multiple Place Destructible Mesh"))
class PROCEDURAL_API AMultiplePlaceDMesh : public AMultiplePlaceObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (DisplayName = "Destructible Mesh"))
    TAssetPtr<UDestructibleMesh> DMeshAsset;

#if WITH_EDITOR
    UPROPERTY()
    TArray<UStaticMeshComponent*> PreviewComponents;
#endif

    // Sets default values for this character's properties
    AMultiplePlaceDMesh();

protected:
#if WITH_EDITOR
    virtual void SetupPreview(const FTransform Transform, int SplinePoint);
    virtual void ClearPreviews();
#endif

    virtual void SetupFirstObject(const FTransform Transform);
    virtual void SetupOtherObject(const FTransform Transform, int Id);
};
