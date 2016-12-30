// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "CustomVolume.h"

ACustomVolume::ACustomVolume(const FObjectInitializer& ObjectInitializer)
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

void ACustomVolume::OnConstruction(const FTransform & Transform)
{
    Super::OnConstruction(Transform);
    //Update Color On Updated
    BrushColor = Color;
    bSolidWhenSelected = bSolid;
}

#if WITH_EDITOR
void ACustomVolume::LoadedFromAnotherClass(const FName& OldClassName)
{
    Super::LoadedFromAnotherClass(OldClassName);

    if (GetLinkerUE4Version() < VER_UE4_REMOVE_DYNAMIC_VOLUME_CLASSES)
    {
        static FName DynamicTriggerVolume_NAME(TEXT("DynamicTriggerVolume"));

        if (OldClassName == DynamicTriggerVolume_NAME)
        {
            GetBrushComponent()->Mobility = EComponentMobility::Movable;
        }
    }
}
#endif

void ACustomVolume::NotifyActorBeginOverlap(AActor * OtherActor)
{
    if (AEntity* Entity = Cast<AEntity>(OtherActor)) {
        if (Entity->IsA(EntityFilter)) {
            EntityEnter(Entity);
        }
    }
    Super::NotifyActorBeginOverlap(OtherActor);
}

void ACustomVolume::NotifyActorEndOverlap(AActor * OtherActor)
{
    if (AEntity* Entity = Cast<AEntity>(OtherActor)) {
        if (Entity->IsA(EntityFilter)) {
            EntityExit(Entity);
        }
    }
    Super::NotifyActorBeginOverlap(OtherActor);
}

void ACustomVolume::EntityEnter(AEntity * Entity)
{
    ReceiveEntityEnter(Entity);
    OnEntityEnter.Broadcast(Entity);
}

void ACustomVolume::EntityExit(AEntity * Entity)
{
    ReceiveEntityExit(Entity);
    OnEntityExit.Broadcast(Entity);
}
