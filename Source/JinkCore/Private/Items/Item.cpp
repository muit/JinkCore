// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Buff.h"
#include "Entity.h"
#include "Item.h"

#define LOCTEXT_NAMESPACE "Item"

UItem::UItem(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    DisplayName = "Item";
    Description = LOCTEXT("Description", "");


#if WITH_EDITORONLY_DATA
    DesignerNotes = LOCTEXT("DesignerNotes", "");
#endif

    //Properties
    DamageIncrement = 0;
    LiveIncrement = 0;
    MovementSpeedCof = 1;
    FireRateCof = 1;
    BulletSpeedCof = 1;
}

void UItem::PickUp(AEntity * Owner)
{
    if (Owner) {
        Holder = Owner;

        //Registry Holder Dead event
        Holder->JustDiedDelegate.AddDynamic(this, &UItem::HolderJustDied);

        //Apply Buffs
        for (auto& BuffType : BuffsApplied) {
            if (UBuff* NewBuff = Holder->ApplyBuff(BuffType)) {
                BuffsAppliedObjects.Add(NewBuff);
            }
        }

        OnPickUp(Holder);
    }
}

void UItem::Drop() {
    OnDrop();

    //Remove Buffs
    for (auto* Buff : BuffsAppliedObjects) {
        Holder->RemoveBuff(Buff);
    }

    MarkPendingKill();
}

void UItem::OnPickUp_Implementation(AEntity * Entity)
{
}

void UItem::OnDrop_Implementation()
{
}

void UItem::HolderJustDied_Implementation(AController * InstigatedBy, AEntity * Killer)
{
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
