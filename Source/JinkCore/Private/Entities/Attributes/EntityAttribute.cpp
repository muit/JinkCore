// Fill out your copyright notice in the Description page of Project Settings.

#include "JinkCorePrivatePCH.h"

#include "Entity.h"

#include "EntityAttribute.h"

const float FEntityAttribute::Calculate() const
{
    float ActualValue = BaseValue;

    for (auto& Mod : Modifications)
    {
        Mod.Apply(*this, ActualValue);
    }

    if (Owner) {
        for (auto* Buff : Owner->Buffs)
        {
            ActualValue = Buff->OnAttributeValue(*this, ActualValue);
        }
    }

    return ActualValue;
}
