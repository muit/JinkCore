// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"

#include "Entity.h"

#include "EntityAttribute.h"

void FEntityAttribute::AddModification(FAttributeModification& Modification) {
    Modifications.Add(Modification);
    OnModified.ExecuteIfBound(AO_Add, Modification);
}

void FEntityAttribute::RemoveModification(FAttributeModification& Modification) {
    Modifications.Remove(Modification);
    OnModified.ExecuteIfBound(AO_Remove, Modification);
}

void FEntityAttribute::AddBuffModification(FAttributeModification& Modification) {
    BuffModifications.Add(Modification);
    OnModified.ExecuteIfBound(AO_AddBuff, Modification);
}

void FEntityAttribute::RemoveBuffModification(FAttributeModification& Modification) {
    BuffModifications.Remove(Modification);
    OnModified.ExecuteIfBound(AO_RemoveBuff, Modification);
}

const float FEntityAttribute::GetValue() const
{
    float ActualValue = BaseValue;

    for (auto& Mod : Modifications)
    {
        Mod.Apply(*this, ActualValue);
    }

    for (auto& BuffMod : BuffModifications) {
        BuffMod.Apply(*this, ActualValue);
    }

    return ActualValue;
}
