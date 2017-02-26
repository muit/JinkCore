// Fill out your copyright notice in the Description page of Project Settings.

#include "JinkCorePrivatePCH.h"
#include "Buff.h"

#define LOCTEXT_NAMESPACE "Buff"

UBuff::UBuff(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    Category = EBuffCategory::BC_Buff;
}

float UBuff::OnAttributeValue_Implementation(const FEntityAttribute& Attribute, const float& LastValue)
{
    return LastValue;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
