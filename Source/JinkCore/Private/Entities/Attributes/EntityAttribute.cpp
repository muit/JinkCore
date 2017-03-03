// Fill out your copyright notice in the Description page of Project Settings.

#include "JinkCorePrivatePCH.h"

#include "Entity.h"

#include "EntityAttribute.h"

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
