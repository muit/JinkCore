// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AttributeModification.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct JINKCORE_API FAttributeModification
{
    GENERATED_BODY()

    FAttributeModification() {
        BasePercentageIncrement = 0;
        PercentageIncrement = 0;
        ScalarIncrement = 0;
    }

    /* Adds a percentage of the base value to the attribute */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (UIMin = "0.0", UIMax = "100.0"))
    float BasePercentageIncrement;

    /* Adds a percentage of the last value to the attribute */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (UIMin = "0.0", UIMax = "100.0"))
    float PercentageIncrement;

    /* Sums an amount to the attribute */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float ScalarIncrement;
};
