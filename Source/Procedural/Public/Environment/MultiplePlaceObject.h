// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "MultiplePlaceObject.generated.h"

UCLASS(Abstract, hideCategories = (Input, Rendering))
class PROCEDURAL_API AMultiplePlaceObject : public AActor
{
    GENERATED_BODY()
    
public:

    UBillboardComponent* SpriteComponent;
    // Icon sprite
    UTexture2D* SpriteTexture;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline", meta = (DisplayName = "Spline"))
    USplineComponent* SplineComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ClampMin = "1"))
    int Amount;

    UPROPERTY()
    bool bDestroyAfterSpawn;

#if WITH_EDITOR
    UPROPERTY(Transient)
    UMaterialInterface* PreviewMaterial;
#endif

    // Sets default values for this character's properties
    AMultiplePlaceObject();

    virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR  
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    virtual void BeginPlay() override;

protected:
#if WITH_EDITOR
    virtual void SetupPreview(const FTransform Transform, int SplinePoint) {}
    virtual void ClearPreviews() {}
#endif

    virtual void SetupFirstObject(const FTransform Transform) {}
    virtual void SetupOtherObject(const FTransform Transform, int Id) {}
};