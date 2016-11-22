// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstanceBounds.h"
#include "LIAnchorViewerComponent.h"


// Sets default values for this component's properties
ULIAnchorViewerComponent::ULIAnchorViewerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULIAnchorViewerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULIAnchorViewerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

#if WITH_EDITOR
void ULIAnchorViewerComponent::PostEditComponentMove(bool bFinished) {
	if (ALevelInstanceBounds* LIBoundsActor = GetLIBoundsActor()) {
		if (FLIAnchor* Anchor = GetLIAnchor()) {
			Anchor->Transform = GetComponentTransform();
			//Save to level instance asset
			LIBoundsActor->UpdateAnchors();
		}
	}
}

void ULIAnchorViewerComponent::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != NULL) {
		FName PropName = PropertyChangedEvent.Property->GetFName();

		if (PropName == GET_MEMBER_NAME_CHECKED(ULIAnchorViewerComponent, Name)) {
			//On name change, update anchors on level bounds
			if (ALevelInstanceBounds* LIBoundsActor = GetLIBoundsActor()) {
				if (FLIAnchor* Anchor = GetLIAnchor()) {
					Anchor->Name = Name;
					//Save to level instance asset
					LIBoundsActor->UpdateAnchors();
				}
			}
		}
	}
}
#endif //WITH_EDITOR

