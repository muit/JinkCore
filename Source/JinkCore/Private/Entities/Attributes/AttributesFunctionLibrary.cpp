// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AttributesFunctionLibrary.h"

bool UAttributesFunctionLibrary::Is(UPARAM(ref) FEntityAttribute & A, UPARAM(ref) FEntityAttribute & B)
{
    return A == B;
}
bool UAttributesFunctionLibrary::Equals(UPARAM(ref) FEntityAttribute & A, UPARAM(ref) FEntityAttribute & B)
{
    return A.BaseValue == B.BaseValue;
}

const float UAttributesFunctionLibrary::GetValue (const FEntityAttribute & Attribute)
{
    return Attribute.GetValue();
}

const float UAttributesFunctionLibrary::ToFloat (const FEntityAttribute & Attribute)
{
    return GetValue(Attribute);
}

const float UAttributesFunctionLibrary::GetBase (const FEntityAttribute & Attribute)
{
    return Attribute.BaseValue;
}

void UAttributesFunctionLibrary::SetBase(UPARAM(ref) FEntityAttribute & Attribute, float Value)
{
    Attribute.BaseValue = Value;
}

void UAttributesFunctionLibrary::AddModification(FEntityAttribute & Attribute, FAttributeModification & Modification)
{
    Attribute.AddModification(Modification);
}

/*
void UAttributesFunctionLibrary::AddLinkedModification(FEntityAttribute& Attribute, FAttributeModification& Modification, AActor* Owner) {
Modification.Owner = Owner;
Modification.bLinkedToOwner = true;
Attribute.AddModification(Modification);
}*/

void UAttributesFunctionLibrary::RemoveModification(FEntityAttribute & Attribute, FAttributeModification & Modification)
{
    Attribute.RemoveModification(Modification);
}

const TArray<FAttributeModification>& UAttributesFunctionLibrary::GetModifications(UPARAM(ref) FEntityAttribute& Attribute) {
    return Attribute.GetModifications();
}

void UAttributesFunctionLibrary::CleanModifications(UPARAM(ref) FEntityAttribute& Attribute) {
    return Attribute.CleanModifications();
}
