// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "EntityAttribute.h"
#include "AttributeModification.h"

#include "Buff.generated.h"

class AEntity;

UENUM(BlueprintType)
enum class EBuffCategory : uint8
{
    BC_Buff 	UMETA(DisplayName = "Buff"),
    BC_Debuff 	UMETA(DisplayName = "Debuff")
};

/**
 * 
 */
UCLASS(Blueprintable)
class JINKCORE_API UBuff : public UObject
{
    GENERATED_UCLASS_BODY()

public:

    void Setup(AEntity* Owner);

    //~ Begin Buff Interface
    UPROPERTY(BlueprintReadOnly, Category = "Buff")
    AEntity* Entity;

    //~ Begin Buff Interface
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
    EBuffCategory Category;

    UFUNCTION(BlueprintNativeEvent, Category = "Buff")
    float OnAttributeValue(const FEntityAttribute& Attribute, const float& ActualValue);
    //~ End Item Interface
	
    UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Buff")
    float ApplyModification(const FEntityAttribute& Attribute, const FAttributeModification& Mod, float ActualValue);
    //~ End Item Interface
};
