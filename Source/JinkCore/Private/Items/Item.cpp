// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Item.h"

#define LOCTEXT_NAMESPACE "Item"

UItem::UItem(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    DisplayName = "Item";
    Description = LOCTEXT("Description", "");

    //Properties
    DamageIncrement = 0;
    LiveIncrement = 0;
    MovementSpeedCof = 1;
    FireRateCof = 1;
    BulletSpeedCof = 1;
}

void UItem::SetHolder(AEntity * Entity)
{
    Holder = Entity;
}

AEntity * UItem::GetHolderEntity()
{
    return Holder;
}

void UItem::HolderJustDied_Implementation(AEntity * Entity, AController * InstigatedBy, AEntity * Killer)
{
}

void UItem::ApplyEntityModifications_Implementation(AEntity * Entity)
{
    //Apply Live increment
    Entity->MaxLive += LiveIncrement;
    Entity->Live += LiveIncrement;

    //Apply Damage Increment
    Entity->Damage += DamageIncrement;
}

void UItem::UndoEntityModifications_Implementation(AEntity * Entity)
{
    //Undo Live increment
    Entity->MaxLive -= LiveIncrement;
    Entity->Live = FMath::Clamp(Entity->Live, 0.0f, Entity->MaxLive);

    //Undo Damage Increment
    Entity->Damage -= DamageIncrement;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
