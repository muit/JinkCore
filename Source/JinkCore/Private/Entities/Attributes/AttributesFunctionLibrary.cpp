// Fill out your copyright notice in the Description page of Project Settings.

#include "JinkCorePrivatePCH.h"
#include "AttributesFunctionLibrary.h"

const float UAttributesFunctionLibrary::GetValue (const FEntityAttribute & Attribute)
{
    return Attribute.Calculate();
}

const float UAttributesFunctionLibrary::ToFloat (const FEntityAttribute & Attribute)
{
    return GetValue(Attribute);
}

const float& UAttributesFunctionLibrary::GetBase (const FEntityAttribute & Attribute)
{
    return Attribute.BaseValue;
}
