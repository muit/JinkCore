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
    DamageMod      = FAttributeModification();
    LiveMod        = FAttributeModification();
    MovementMod    = FAttributeModification();
    FireRateMod    = FAttributeModification();
    BulletSpeedMod = FAttributeModification();
}

void UItem::PickUp(AEntity * Owner)
{
    if (Owner) {
        Holder = Owner;

        //Registry Holder Dead event
        Holder->JustDiedDelegate.AddDynamic(this, &UItem::HolderJustDied);

        //Apply Modifications
        Holder->Damage.AddModification(DamageMod);
        Holder->MaxLive.AddModification(LiveMod);
        Holder->FireRate.AddModification(FireRateMod);
        Holder->BulletSpeed.AddModification(BulletSpeedMod);

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
    if (!Holder)
        return;

    OnDrop();

    //Apply Modifications
    Holder->Damage.RemoveModification(DamageMod);
    Holder->MaxLive.RemoveModification(LiveMod);
    Holder->FireRate.RemoveModification(FireRateMod);
    Holder->BulletSpeed.RemoveModification(BulletSpeedMod);

    //Remove Buffs
    for (auto* Buff : BuffsAppliedObjects) {
        Holder->RemoveBuff(Buff);
    }

    Holder = nullptr;
    MarkPendingKill();
}

void UItem::OnPickUp_Implementation(AEntity * Entity)
{
}

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
