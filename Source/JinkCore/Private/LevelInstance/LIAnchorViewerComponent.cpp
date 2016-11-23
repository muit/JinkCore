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
void ULIAnchorViewerComponent::PostEditComponentMove(bool bFinished) {
	if (ALevelInstanceBounds* LIBoundsActor = GetLIBoundsActor()) {
        FLIAnchor Anchor = GetLIAnchor();
        if(Anchor.GUID == AnchorGUID) {
			//Anchor->Transform = GetComponentTransform();
			//Save to level instance asset
			//LIBoundsActor->UpdateAnchors();
		}
	}
}

void ULIAnchorViewerComponent::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{}
#endif //WITH_EDITOR

