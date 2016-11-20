// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstanceComponent.h"


// Sets default values for this component's properties
ULevelInstanceComponent::ULevelInstanceComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}


// Called when the game starts
void ULevelInstanceComponent::BeginPlay()
{
    Super::BeginPlay();

    // TEMPORAL: Spawn Level on Begin Play
    if (SetLevelInstanceAsset(LevelInstanceAsset)) {
        LevelI_Data->SpawnLevel(GetComponentTransform());
    }
    
}


// Called every frame
void ULevelInstanceComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    // ...
}

