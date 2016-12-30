// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "InteractuableActor.h"


// Sets default values
AInteractuableActor::AInteractuableActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractuableActor::BeginPlay()
{
    Super::BeginPlay();
    
}

// Called every frame
void AInteractuableActor::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );

}

