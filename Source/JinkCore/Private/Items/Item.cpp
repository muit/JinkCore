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

    bUnique = false;
    bUsable = false;
    bStackable = false;
    Count = 0;

    //Properties
    DamageMod      = FAttributeModification();
    LiveMod        = FAttributeModification();
    MovementMod    = FAttributeModification();
    FireRateMod    = FAttributeModification();
    BulletSpeedMod = FAttributeModification();
}

void UItem::PickUp(AEntity * Owner)
{
    if (IsPickedUp() && !bUnique && bStackable) {
        ++Count;
    }

    if (!IsPickedUp() && Owner) {
        Holder = Owner;

        //Registry Holder Dead event
        Holder->JustDiedDelegate.AddDynamic(this, &UItem::HolderJustDied);

        //Apply Modifications
        Holder->Damage.AddModification(DamageMod);
        Holder->FireRate.AddModification(FireRateMod);
        Holder->BulletSpeed.AddModification(BulletSpeedMod);

        //Update MaxLive Modifications and Live value
        const float OldMaxLive = Holder->MaxLive;
        Holder->MaxLive.AddModification(LiveMod);
        const float NewMaxLive = Holder->MaxLive;
        const float PositiveHealthIncrement = FMath::Max(0.0f, NewMaxLive - OldMaxLive);
        Holder->Live = FMath::Clamp(Holder->Live + PositiveHealthIncrement, 0.0f, NewMaxLive);

        //Apply Buffs
        for (auto& BuffType : BuffsApplied) {
            if (UBuff* NewBuff = Holder->ApplyBuff(BuffType)) {
                BuffsAppliedObjects.Add(NewBuff);
            }
        }

        ++Count;
        OnPickUp(Holder);
    }
}

void UItem::Drop() {
    if (!Holder)
        return;

    OnDrop();

    //Apply Modifications
    Holder->Damage.RemoveModification(DamageMod);
    Holder->MaxLive.RemoveModification(LiveMod);
    Holder->FireRate.RemoveModification(FireRateMod);
    Holder->BulletSpeed.RemoveModification(BulletSpeedMod);

    //Update Live
    Holder->Live = FMath::Clamp(Holder->Live, 0.0f, (float)Holder->MaxLive);

    //Remove Buffs
    for (auto* Buff : BuffsAppliedObjects) {
        Holder->RemoveBuff(Buff);
    }

    Holder = nullptr;
    MarkPendingKill();
}

void UItem::OnPickUp_Implementation(AEntity * Entity)
{}

void UItem::OnDrop_Implementation()
{}

void UItem::NotifyFired_Implementation(bool& canFire) {
    canFire = true;
}

void UItem::HolderJustDied_Implementation(AController * InstigatedBy, AEntity * Killer)
{
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
