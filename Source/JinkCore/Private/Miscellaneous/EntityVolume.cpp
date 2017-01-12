// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "EntityVolume.h"

AEntityVolume::AEntityVolume(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer) {

    static FName CollisionProfileName(TEXT("Trigger"));
    GetBrushComponent()->SetCollisionProfileName(CollisionProfileName);

    bColored = true;

    Color.R = 100;
    Color.G = 255;
    Color.B = 100;
    Color.A = 125;
    BrushColor = Color;

    EntityFilter = AEntity::StaticClass();
}

void AEntityVolume::OnConstruction(const FTransform & Transform)
{
    Super::OnConstruction(Transform);
    //Update Color On Updated
    BrushColor = Color;
    bSolidWhenSelected = bSolid;
}

void AEntityVolume::NotifyActorBeginOverlap(AActor * OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (AEntity* Entity = Cast<AEntity>(OtherActor)) {
        if (Entity->IsA(EntityFilter)) {
            EntityEnter(Entity);
        }
    }
}

void AEntityVolume::NotifyActorEndOverlap(AActor * OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (AEntity* Entity = Cast<AEntity>(OtherActor)) {
        if (Entity->IsA(EntityFilter)) {
            EntityExit(Entity);
        }
    }
}

void AEntityVolume::EntityEnter(AEntity * Entity)
{
    ReceiveEntityEnter(Entity);
    OnEntityEnter.Broadcast(Entity);
}

void AEntityVolume::EntityExit(AEntity * Entity)
{
    ReceiveEntityExit(Entity);
    OnEntityExit.Broadcast(Entity);
}
