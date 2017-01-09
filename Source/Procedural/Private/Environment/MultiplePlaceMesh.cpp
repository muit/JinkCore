#include "ProceduralPrivatePCH.h"

#include "MultiplePlaceMesh.h"

static ConstructorHelpers::FObjectFinderOptional<UTexture2D> ModuleTextureObject = TEXT("/Engine/EditorResources/S_Actor");

AMultiplePlaceMesh::AMultiplePlaceMesh() {

    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = SceneComponent;

#if WITH_EDITORONLY_DATA
    SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
    if (SpriteComponent)
    {
        SpriteComponent->Sprite = ModuleTextureObject.Get(); // Get the sprite texture from helper class object
        SpriteComponent->SpriteInfo.DisplayName = LOCTEXT("Icon", "Icon");    // Assign sprite display name
        SpriteComponent->SetupAttachment(RootComponent);     // Attach sprite to scene component
    }
#endif // WITH_EDITORONLY_DATA

    //Setup Spline Component
    SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
    SplineComponent->SetupAttachment(RootComponent);
    SplineComponent->SetUnselectedSplineSegmentColor(FLinearColor(91, 185, 239));
    SplineComponent->SetDrawDebug(false);

    //Setup Mesh Component
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MeshComponent->SetupAttachment(RootComponent);

    if (Mesh) {
        MeshComponent->SetStaticMesh(Mesh);
    }

#if WITH_EDITORONLY_DATA
    //If it's in editor, don't show main mesh
    MeshComponent->SetVisibility(false);

    //Load Mesh Preview material
    PreviewMaterial = LoadObject<UMaterial>(nullptr, TEXT("/JinkCore/Procedural/M_MeshPreview.M_MeshPreview"), nullptr, LOAD_None, nullptr);
#endif
}

void AMultiplePlaceMesh::OnConstruction(const FTransform & Transform)
{
    //For each spline point
    const int PointCount = SplineComponent->GetNumberOfSplinePoints();
    for (int i = 0; i < PointCount; i++)
    {
        //Reset every point's tangent in the spline to zero. This creates straight lines.
        SplineComponent->SetTangentAtSplinePoint(i, FVector::ZeroVector, ESplineCoordinateSpace::Local);

#if WITH_EDITORONLY_DATA
        //Show Mesh Preview
        UStaticMeshComponent* PreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Preview"));
        PreviewMesh->SetupAttachment(SplineComponent);

        //Move to point
        const FTransform PointTransform = SplineComponent->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::World);
        PreviewMesh->SetWorldTransform(PointTransform);

        PreviewMesh->SetStaticMesh(MeshComponent->GetStaticMesh());
        PreviewMesh->SetMaterial(0, PreviewMaterial);
        PreviewMeshComponents->Add(PreviewMesh);
#endif
    }
}

void AMultiplePlaceMesh::BeginPlay()
{
    const int SelectedPoint = FMath::RandRange(0, SplineComponent->GetNumberOfSplinePoints());
    const FTransform PointTransform = SplineComponent->GetTransformAtSplinePoint(SelectedPoint, ESplineCoordinateSpace::World);

    //Move Mesh to a random point
    MeshComponent->SetWorldTransform(PointTransform);
}

FRandomStream& AMultiplePlaceMesh::GetSeed_Implementation()
{
    return FRandomStream();
}
