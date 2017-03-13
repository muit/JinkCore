// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "ProceduralPrivatePCH.h"
#include "MultiplePlaceMesh.h"
#include "Runtime/Launch//Resources/Version.h"

#define LOCTEXT_NAMESPACE "MultiplePlaceMesh" 

static ConstructorHelpers::FObjectFinderOptional<UTexture2D> MultiplePlaceMeshTexture = TEXT("/Engine/EditorResources/S_Actor");

AMultiplePlaceMesh::AMultiplePlaceMesh() {
    //Setup Mesh Component
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MeshComponent->SetupAttachment(RootComponent);

#if WITH_EDITOR
    //If it's in editor, don't show main mesh
    MeshComponent->SetVisibility(false);
#endif
}

void AMultiplePlaceMesh::OnConstruction(const FTransform & Transform)
{
    Super::OnConstruction(Transform);

    if (Mesh) {
        MeshComponent->SetStaticMesh(Mesh);
    }
}

#if WITH_EDITOR
void AMultiplePlaceMesh::SetupPreview(const FTransform Transform, int SplinePoint)
{
#if ENGINE_MINOR_VERSION >= 14 //If engine is 4.14 or newer
    UStaticMesh* TemplateMesh = MeshComponent->GetStaticMesh();
#else
    UStaticMesh* TemplateMesh = MeshComponent->StaticMesh;
#endif

    if (TemplateMesh) {
        //Create & Show Mesh Preview
        UStaticMeshComponent* PreviewMeshC = NewObject<UStaticMeshComponent>(this, FName(*("Preview_" + FString::FromInt(SplinePoint))));
        PreviewMeshC->RegisterComponent();
        PreviewMeshC->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

        //Move to point
        PreviewMeshC->SetWorldTransform(Transform);

        PreviewMeshC->SetStaticMesh(TemplateMesh);
        PreviewMeshC->SetMaterial(0, PreviewMaterial);
        PreviewMeshComponents.Add(PreviewMeshC);
    }
}

void AMultiplePlaceMesh::ClearPreviews()
{
    for (auto* MeshC : PreviewMeshComponents) {
        MeshC->DestroyComponent();
    }
    PreviewMeshComponents.Empty();
}
#endif

void AMultiplePlaceMesh::SetupFirstObject(const FTransform Transform)
{
    MeshComponent->SetWorldTransform(Transform);
    MeshComponent->SetVisibility(true);
}

void AMultiplePlaceMesh::SetupOtherObject(const FTransform Transform, int Id)
{
    //Make a copy from MeshComponent
    UStaticMeshComponent* PreviewMesh = NewObject<UStaticMeshComponent>(this, FName(*("Mesh_" + FString::FromInt(Id))), RF_NoFlags, MeshComponent);
    PreviewMesh->SetWorldTransform(Transform);

    PreviewMesh->RegisterComponent();
    PreviewMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

#undef LOCTEXT_NAMESPACE 
