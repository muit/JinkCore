#include "ProceduralPrivatePCH.h"

#include "MultiplePlaceMesh.h"

#define LOCTEXT_NAMESPACE "MultiplePlaceMesh" 

static ConstructorHelpers::FObjectFinderOptional<UTexture2D> MultiplePlaceMeshTexture = TEXT("/Engine/EditorResources/S_Actor");

AMultiplePlaceMesh::AMultiplePlaceMesh() {
    MeshAmount = 1;

    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = SceneComponent;

#if WITH_EDITORONLY_DATA
    SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
    if (SpriteComponent)
    {
        SpriteComponent->Sprite = MultiplePlaceMeshTexture.Get(); // Get the sprite texture from helper class object
        SpriteComponent->SpriteInfo.DisplayName = LOCTEXT("Icon", "Icon");    // Assign sprite display name
        SpriteComponent->SetupAttachment(RootComponent);     // Attach sprite to scene component
    }
#endif // WITH_EDITORONLY_DATA

    //Setup Spline Component
    SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
    SplineComponent->SetupAttachment(RootComponent);
    SplineComponent->SetUnselectedSplineSegmentColor(FLinearColor(0.0f, 0.725f, 1.0f));
    SplineComponent->SetDrawDebug(false);

    //Setup Mesh Component
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MeshComponent->SetupAttachment(RootComponent);

#if WITH_EDITORONLY_DATA
    //If it's in editor, don't show main mesh
    MeshComponent->SetVisibility(false);

    //Load Mesh Preview material
    PreviewMaterial = LoadObject<UMaterial>(nullptr, TEXT("/JinkCore/Procedural/M_MeshPreview.M_MeshPreview"), nullptr, LOAD_None, nullptr);
#endif
}

void AMultiplePlaceMesh::OnConstruction(const FTransform & Transform)
{
    if (Mesh) {
        MeshComponent->SetStaticMesh(Mesh);
    }

    //Remove Old Components
    ClearPreviews();

    //For each spline point
    const int PointCount = SplineComponent->GetNumberOfSplinePoints();
    for (int i = 0; i < PointCount; i++)
    {
        //Reset every point's tangent in the spline to zero. This creates straight lines.
        SplineComponent->SetTangentAtSplinePoint(i, FVector::ZeroVector, ESplineCoordinateSpace::Local);

#if WITH_EDITORONLY_DATA
        if(MeshComponent->GetStaticMesh()) {
            //Show Mesh Preview
            UStaticMeshComponent* PreviewMesh = NewObject<UStaticMeshComponent>(this, FName(*("Preview_" + FString::FromInt(i))));
            PreviewMesh->RegisterComponent();
            PreviewMesh->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

            //Move to point
            const FTransform PointTransform = SplineComponent->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::World);
            PreviewMesh->SetWorldTransform(PointTransform);

            PreviewMesh->SetStaticMesh(MeshComponent->GetStaticMesh());
            PreviewMesh->SetMaterial(0, PreviewMaterial);
            PreviewMeshComponents.Add(PreviewMesh);
        }
#endif
    }
}

#if WITH_EDITOR  
void AMultiplePlaceMesh::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.Property) {
        //Get the name of the property that was changed  
        FName PropertyName = PropertyChangedEvent.Property->GetFName();

        // We test using GET_MEMBER_NAME_CHECKED so that if someone changes the property name  
        // in the future this will fail to compile and we can update it.  
        if (PropertyName == GET_MEMBER_NAME_CHECKED(AMultiplePlaceMesh, MeshAmount))
        {
            const int PointCount = SplineComponent->GetNumberOfSplinePoints();
            if(MeshAmount > PointCount) {
                MeshAmount = PointCount;
                PropertyChangedEvent.Property->MarkPackageDirty();
            }
        }
    }

    // Call the base class version  
    Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif 

void AMultiplePlaceMesh::BeginPlay()
{
    //Destroy Previews & Spline
    ClearPreviews();
    SplineComponent->DestroyComponent();


    const int PointCount = SplineComponent->GetNumberOfSplinePoints();

    //Register all points to avoid a mesh spawn in the same point.
    TArray<int> PointIdx;
    for (int i = 0; i < PointCount; i++)
    { PointIdx.Add(i); }
    
    {
        //Move First Mesh to a random point
        const int SelectedPoint = FMath::RandRange(0, PointCount);
        PointIdx.Remove(SelectedPoint);
        const FTransform PointTransform = SplineComponent->GetTransformAtSplinePoint(SelectedPoint, ESplineCoordinateSpace::World);
        MeshComponent->SetWorldTransform(PointTransform);
        MeshComponent->SetVisibility(true);
    }

    if (MeshAmount > 1) {
        for (int i = 1; i < FMath::Min(MeshAmount, PointCount); i++) {
            //Make a copy from MeshComponent
            UStaticMeshComponent* PreviewMesh = NewObject<UStaticMeshComponent>(this, FName(*("Mesh_" + FString::FromInt(i))), RF_NoFlags, MeshComponent);

            //Make sure that the Id is not been used
            const int SelectedPoint = PointIdx[FMath::RandRange(0, PointIdx.Num()-1)];
            PointIdx.Remove(SelectedPoint);

            const FTransform PointTransform = SplineComponent->GetTransformAtSplinePoint(SelectedPoint, ESplineCoordinateSpace::World);
            MeshComponent->SetWorldTransform(PointTransform);

            PreviewMesh->RegisterComponent();
            PreviewMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        }
    }
}

void AMultiplePlaceMesh::ClearPreviews()
{
    for (auto* MeshC : PreviewMeshComponents) {
        MeshC->DestroyComponent();
    }
    PreviewMeshComponents.Empty();
}

FRandomStream AMultiplePlaceMesh::GetSeed_Implementation()
{
    return FRandomStream();
}

#undef LOCTEXT_NAMESPACE 
