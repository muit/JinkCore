// Fill out your copyright notice in the Description page of Project Settings.

#include "JinkCorePrivatePCH.h"
#include "Entity.h"
#include "Buff.h"

#define LOCTEXT_NAMESPACE "Buff"

UBuff::UBuff(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    Category = EBuffCategory::BC_Buff;
}

void UBuff::Setup(AEntity* Owner) {
    Entity = Owner;
}

float UBuff::OnAttributeValue_Implementation(const FEntityAttribute& Attribute, const float& ActualValue)
{
    return ActualValue;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

float UBuff::ApplyModification(const FEntityAttribute& Attribute, const FAttributeModification& Mod, float ActualValue)
{
    Mod.Apply(Attribute, ActualValue);
    return ActualValue;
}
