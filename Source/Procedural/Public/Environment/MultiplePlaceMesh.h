// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "MultiplePlaceMesh.generated.h"

UCLASS(Blueprintable, hideCategories = (Input, Rendering))
class PROCEDURAL_API AMultiplePlaceMesh : public AActor
{
    GENERATED_BODY()
    
public:

    UBillboardComponent* SpriteComponent;
    // Icon sprite
    UTexture2D* SpriteTexture;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline", meta = (DisplayName = "Spline"))
    USplineComponent* SplineComponent;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (DisplayName = "Mesh"))
    UStaticMeshComponent* MeshComponent;
    UPROPERTY()
    TArray<UStaticMeshComponent*> MoreMeshComponents;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
    UStaticMesh* Mesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ClampMin = "1"))
    int MeshAmount;

#if WITH_EDITORONLY_DATA
    UPROPERTY(Transient)
    UMaterialInterface* PreviewMaterial;
    UPROPERTY()
    TArray<UStaticMeshComponent*> PreviewMeshComponents;
#endif

    // Sets default values for this character's properties
    AMultiplePlaceMesh();

    virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR  
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

protected:
    void ClearPreviews();

    UFUNCTION(BlueprintNativeEvent)
    FRandomStream GetSeed();
};