// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "MultiplePlaceObject.h"
#include "MultiplePlaceMesh.generated.h"

UCLASS(Blueprintable)
class PROCEDURAL_API AMultiplePlaceMesh : public AMultiplePlaceObject
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
    UStaticMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (DisplayName = "Mesh"))
    UStaticMeshComponent* MeshComponent;
    UPROPERTY()
    TArray<UStaticMeshComponent*> MoreMeshComponents;
#if WITH_EDITORONLY_DATA
    UPROPERTY()
    TArray<UStaticMeshComponent*> PreviewMeshComponents;
#endif

    // Sets default values for this character's properties
    AMultiplePlaceMesh();

    virtual void OnConstruction(const FTransform& Transform) override;

protected:
#if WITH_EDITORONLY_DATA
    virtual void SetupPreview(const FTransform Transform, int SplinePoint);
    virtual void ClearPreviews();
#endif

    virtual void SetupFirstObject(const FTransform Transform);
    virtual void SetupOtherObject(const FTransform Transform, int Id);
};