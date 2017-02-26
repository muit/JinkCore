// Fill out your copyright notice in the Description page of Project Settings.

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
        //bLinkedToOwner = false;
        Guid = FGuid::NewGuid();
    }
    /*
    FAttributeModification(UObject* _Owner, bool LinkedToOwner = false) : BasePercentageIncrement(0), PercentageIncrement(0), ScalarIncrement(0)
    {
        Owner = _Owner;
        bLinkedToOwner = true;
    }*/

public:
    /*
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    UObject* Owner;

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    bool bLinkedToOwner;*/

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
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
