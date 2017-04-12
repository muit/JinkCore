// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Entity.h"
#include "Item.h"
#include "Buff.h"

#define LOCTEXT_NAMESPACE "Buff"

UBuff::UBuff(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    Category = EBuffCategory::BC_Buff;
}

void UBuff::Apply(AEntity* Owner) {
    Entity = Owner;
    OnApply();
}

void UBuff::Unapply()
{
    OnUnapply();
}

bool UBuff::CanPickUpItem(TSubclassOf<UItem>& Type)
{
    for (auto& RestrictedType : RestrictedItemsForPickUp) {
        if(Type.Get() == RestrictedType.Get()) {
            return false;
        }
    }

    return _CanPickUpItem(Type);
}

bool UBuff::_CanPickUpItem_Implementation(TSubclassOf<UItem>& Type)
{
    return true;
}


void UBuff::ApplyBuffModification(UPARAM(ref) FEntityAttribute& Attribute, UPARAM(ref) FAttributeModification& Mod)
{
    Attribute.AddBuffModification(Mod);
}

void UBuff::RemoveBuffModification(UPARAM(ref) FEntityAttribute& Attribute, UPARAM(ref) FAttributeModification& Mod)
{
    Attribute.RemoveBuffModification(Mod);
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
