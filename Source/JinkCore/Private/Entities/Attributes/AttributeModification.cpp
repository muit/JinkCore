// Fill out your copyright notice in the Description page of Project Settings.

#include "JinkCorePrivatePCH.h"

#include "AttributeModification.h"
#include "EntityAttribute.h"

void FAttributeModification::Apply(const FEntityAttribute& Attribute, float& ActualValue) const{
    if (PercentageIncrement > 0) {
        ActualValue *= 1+(PercentageIncrement / 100);
    }

    if (BasePercentageIncrement > 0) {
        ActualValue += Attribute.BaseValue * (BasePercentageIncrement / 100);
    }

    ActualValue += ScalarIncrement;
}