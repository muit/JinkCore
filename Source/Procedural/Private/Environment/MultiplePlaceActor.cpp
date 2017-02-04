// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "ProceduralPrivatePCH.h"
#include "MultiplePlaceActor.h"

#define LOCTEXT_NAMESPACE "MultiplePlaceActor" 

AMultiplePlaceActor::AMultiplePlaceActor() {
    bDestroyAfterSpawn = true;
}

#if WITH_EDITORONLY_DATA
void AMultiplePlaceActor::SetupPreview(const FTransform Transform, int SplinePoint)
{
    if (ActorClass) {
        //Show Mesh Preview
        UChildActorComponent* Preview = NewObject<UChildActorComponent>(this, FName(*("Preview_" + FString::FromInt(SplinePoint))));
        Preview->RegisterComponent();
        Preview->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

        //Move to point
        Preview->SetWorldTransform(Transform);

        Preview->SetChildActorClass(ActorClass);

        //Set preview material to all actor meshes
        TArray<UActorComponent*> Meshes = Preview->GetChildActor()->GetComponentsByClass(UStaticMeshComponent::StaticClass());
        for (auto* ActorC : Meshes) {
            if (UStaticMeshComponent* MeshC = Cast<UStaticMeshComponent>(ActorC)) {
                MeshC->SetMaterial(0, PreviewMaterial);
            }
        }

        PreviewComponents.Add(Preview);
    }
}

void AMultiplePlaceActor::ClearPreviews()
{
    for (auto* MeshC : PreviewComponents) {
        MeshC->DestroyComponent();
    }
    PreviewComponents.Empty();
}
#endif

void AMultiplePlaceActor::SetupFirstObject(const FTransform Transform)
{
    SetupOtherObject(Transform, 0);
}

void AMultiplePlaceActor::SetupOtherObject(const FTransform Transform, int Id)
{
    if(ActorClass) {
        GetWorld()->SpawnActor(ActorClass, &Transform);
    }
}

#undef LOCTEXT_NAMESPACE 
