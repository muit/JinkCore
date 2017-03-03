#include "ProceduralPrivatePCH.h"

#include "MultiplePlaceObject.h"

#define LOCTEXT_NAMESPACE "MultiplePlaceObject" 

static ConstructorHelpers::FObjectFinderOptional<UTexture2D> MultiplePlaceObjectTexture = TEXT("/Engine/EditorResources/S_Actor");

AMultiplePlaceObject::AMultiplePlaceObject() {
    Amount = 1;
    bDestroyAfterSpawn = false;

    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = SceneComponent;

#if WITH_EDITOR
    SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
    if (SpriteComponent)
    {
        SpriteComponent->Sprite = MultiplePlaceObjectTexture.Get(); // Get the sprite texture from helper class object
        SpriteComponent->SpriteInfo.DisplayName = LOCTEXT("Icon", "Icon");    // Assign sprite display name
        SpriteComponent->SetupAttachment(RootComponent);     // Attach sprite to scene component
    }
#endif // WITH_EDITORONLY_DATA

    //Setup Spline Component
    SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
    SplineComponent->SetupAttachment(RootComponent);
    SplineComponent->SetUnselectedSplineSegmentColor(FLinearColor(0.0f, 0.725f, 1.0f));
    SplineComponent->SetDrawDebug(false);

#if WITH_EDITOR
    //Load Mesh Preview material
    PreviewMaterial = LoadObject<UMaterial>(nullptr, TEXT("/JinkCore/Procedural/M_MeshPreview.M_MeshPreview"), nullptr, LOAD_None, nullptr);
#endif
}

void AMultiplePlaceObject::OnConstruction(const FTransform & Transform)
{
#if WITH_EDITOR
    //Remove Old Preview Components
    ClearPreviews();
#endif

    //For each spline point
    const int PointCount = SplineComponent->GetNumberOfSplinePoints();
    for (int i = 0; i < PointCount; i++)
    {
        //Reset every point's tangent in the spline to zero. This creates straight lines.
        SplineComponent->SetTangentAtSplinePoint(i, FVector::ZeroVector, ESplineCoordinateSpace::Local);

#if WITH_EDITOR
        const FTransform PointTransform = SplineComponent->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::World);
        SetupPreview(PointTransform, i);
#endif
    }
}

#if WITH_EDITOR  
void AMultiplePlaceObject::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.Property) {
        //Get the name of the property that was changed  
        FName PropertyName = PropertyChangedEvent.Property->GetFName();
 
        if (PropertyName == GET_MEMBER_NAME_CHECKED(AMultiplePlaceObject, Amount))
        {
            //Limit MeshAmount from 0 to spline points amount
            const int PointCount = SplineComponent->GetNumberOfSplinePoints();
            if(Amount > PointCount) {
                Amount = PointCount;
                PropertyChangedEvent.Property->MarkPackageDirty();
            }
        }
    }

    // Call the base class version  
    Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif 

void AMultiplePlaceObject::BeginPlay()
{
    //Destroy Previews & Spline
#if WITH_EDITOR
    ClearPreviews();
#endif
    SplineComponent->DestroyComponent();

    const int PointCount = SplineComponent->GetNumberOfSplinePoints();


    //Register all points to avoid a mesh spawn in the same point.
    TArray<int> PointIdx;
    for (int i = 0; i < PointCount; i++) {
        PointIdx.Add(i);
    }
    
    {
        //Move First Mesh to a random point
        const int SelectedPoint = FMath::RandRange(0, PointCount);
        PointIdx.Remove(SelectedPoint);

        const FTransform PointTransform = SplineComponent->GetTransformAtSplinePoint(SelectedPoint, ESplineCoordinateSpace::World);
        SetupFirstObject(PointTransform);
    }

    if (Amount > 1) {
        for (int i = 1; i < FMath::Min(Amount, PointCount); i++) {
            //Make sure that the Id is not been used
            const int SelectedPoint = PointIdx[FMath::RandRange(0, PointIdx.Num()-1)];
            PointIdx.Remove(SelectedPoint);

            const FTransform PointTransform = SplineComponent->GetTransformAtSplinePoint(SelectedPoint, ESplineCoordinateSpace::World);

            SetupOtherObject(PointTransform, i);
        }
    }

    if(bDestroyAfterSpawn) {
        Destroy();
    }
}

#undef LOCTEXT_NAMESPACE 
