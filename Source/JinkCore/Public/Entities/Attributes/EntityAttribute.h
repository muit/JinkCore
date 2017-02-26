// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AttributeModification.h"
#include "EntityAttribute.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct JINKCORE_API FEntityAttribute
{
    GENERATED_BODY()

    FEntityAttribute() {
        BaseValue = 0;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float BaseValue;
	
    UPROPERTY(EditAnywhere, Category = "Attributes")
    TArray<FAttributeModification> Modifications;

    const float Calculate() const
    {
        return 0.0f;
    }
};
