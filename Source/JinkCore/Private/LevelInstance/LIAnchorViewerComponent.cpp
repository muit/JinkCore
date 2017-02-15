// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstanceBounds.h"
#include "LevelInstance.h"
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

void ULIAnchorViewerComponent::SetupAnchorAttachment(ULIAnchorViewerComponent* OtherAnchor, bool IsFather)
{
    if (!OtherAnchor) {
        UE_LOG(LogJinkCore, Warning, TEXT("LevelInstance: Can't setup attachment to an invalid anchor."));
        return;
    }
    
    //Attach this Anchor
    ConnectedAttachment = OtherAnchor;
    bIsAttachmentFather = IsFather;

    //Attach the other Anchor
    OtherAnchor->ConnectedAttachment = this;
    OtherAnchor->bIsAttachmentFather = !IsFather;
}