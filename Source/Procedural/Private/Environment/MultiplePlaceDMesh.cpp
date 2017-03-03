// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "ProceduralPrivatePCH.h"
#include "MultiplePlaceDMesh.h"

#define LOCTEXT_NAMESPACE "MultiplePlaceDMesh" 

AMultiplePlaceDMesh::AMultiplePlaceDMesh() {
    bDestroyAfterSpawn = true;
}

#if WITH_EDITOR
void AMultiplePlaceDMesh::SetupPreview(const FTransform Transform, int SplinePoint)
{
    if (UDestructibleMesh* DMeshObject = DMeshAsset.LoadSynchronous()) {
        //Show Mesh Preview
        UStaticMeshComponent* Preview = NewObject<UStaticMeshComponent>(this, FName(*("Preview_" + FString::FromInt(SplinePoint))));
        Preview->RegisterComponent();
        Preview->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

        //Move to point
        Preview->SetWorldTransform(Transform);

        //Preview the original mesh
        Preview->SetStaticMesh(DMeshObject->SourceStaticMesh);

        //Set preview material to all actor meshes
        Preview->SetMaterial(0, PreviewMaterial);

        PreviewComponents.Add(Preview);
    }
}

void AMultiplePlaceDMesh::ClearPreviews()
{
    for (auto* MeshC : PreviewComponents) {
        MeshC->DestroyComponent();
    }
    PreviewComponents.Empty();
}
#endif

void AMultiplePlaceDMesh::SetupFirstObject(const FTransform Transform)
{
    SetupOtherObject(Transform, 0);
}

void AMultiplePlaceDMesh::SetupOtherObject(const FTransform Transform, int Id)
{
    if (!DMeshAsset.IsNull()) {
        ADestructibleActor* DMesh = CastChecked<ADestructibleActor>(GetWorld()->SpawnActor(ADestructibleActor::StaticClass(), &Transform));
        DMesh->GetDestructibleComponent()->SetDestructibleMesh(DMeshAsset.LoadSynchronous());
    }
}

#undef LOCTEXT_NAMESPACE 
