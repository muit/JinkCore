// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstanceBounds.h"
#include "LevelInstance.h"
#include "LIModule.h"
#include "LIAnchorViewerComponent.h"


// Sets default values for this component's properties
ULIAnchorViewerComponent::ULIAnchorViewerComponent()
    : Super()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
    bIsAttachmentFather = false;
    ConnectedAttachment = nullptr;
    Conector = nullptr;
}


// Called when the game starts
void ULIAnchorViewerComponent::BeginPlay()
{
    Super::BeginPlay();
}


// Called every frame
void ULIAnchorViewerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

#if WITH_EDITOR
void ULIAnchorViewerComponent::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{}
#endif //WITH_EDITOR

void ULIAnchorViewerComponent::SetupAnchorAttachment(ULIAnchorViewerComponent* OtherAnchor, bool bIsFather, bool bSpawnConector)
{
    if (!OtherAnchor) {
        UE_LOG(LogJinkCore, Warning, TEXT("LevelInstance: Can't setup attachment to an invalid anchor."));
        return;
    }
    
    //Attach this Anchor
    ConnectedAttachment = OtherAnchor;
    bIsAttachmentFather = bIsFather;

    //Attach the other Anchor
    OtherAnchor->ConnectedAttachment = this;
    OtherAnchor->bIsAttachmentFather = !bIsFather;


    //Spawn Conector if desired
    if (bSpawnConector) {
        SpawnConector();
    }
}

ALIConector* ULIAnchorViewerComponent::SpawnConector() {
    ULIAnchorViewerComponent* AnchorFather = bIsAttachmentFather || !IsAttached() ? this : ConnectedAttachment;

    if (AnchorFather) {
        FLIAnchorTypeInfo TypeInfo;
        AnchorFather->AnchorData.Type.GetAnchorInfo(TypeInfo);
        FTransform SpawnTransform = AnchorFather->GetComponentTransform();

        //Make connector look backwards
        SpawnTransform.SetRotation(SpawnTransform.Rotator().Add(0.0f, 180.0f, 0.0f).Quaternion());

        ALIConector* ConectorActor = Cast<ALIConector>(GetWorld()->SpawnActor(TypeInfo.GetConectorType(), &SpawnTransform));
        ConectorActor->SetupConAttachment(AnchorFather, ConnectedAttachment);
        return ConectorActor;
    }

    return nullptr;
}
