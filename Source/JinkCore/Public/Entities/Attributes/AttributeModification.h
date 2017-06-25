// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "AttributeModification.generated.h"

struct FEntityAttribute;

/**
 * 
 */
USTRUCT(BlueprintType)
struct JINKCORE_API FAttributeModification
{
    GENERATED_BODY()

    FAttributeModification() : BasePercentageIncrement(0), PercentageIncrement(0), ScalarIncrement(0)
    {
        Guid = FGuid::NewGuid();
    }

    FAttributeModification(float _BasePercentageIncrement, float _PercentageIncrement = 0, float _ScalarIncrement = 0)
        : BasePercentageIncrement(_BasePercentageIncrement), PercentageIncrement(_PercentageIncrement), ScalarIncrement(_ScalarIncrement)
    {
        Guid = FGuid::NewGuid();
    }

public:
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGuid Guid;

    /* Adds a percentage of the base value to the attribute */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (UIMin = "0.0", UIMax = "100.0"))
    float BasePercentageIncrement;

    /* Adds a percentage of the last value to the attribute */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (UIMin = "0.0", UIMax = "100.0"))
    float PercentageIncrement;

    /* Sums an amount to the attribute */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float ScalarIncrement;

    void Apply(const FEntityAttribute& Attribute, float& ActualValue) const;

    //compare two modifications by guid
    FORCEINLINE bool operator==(const FAttributeModification& Other) const
    {
        return Guid == Other.Guid;
    }
};
