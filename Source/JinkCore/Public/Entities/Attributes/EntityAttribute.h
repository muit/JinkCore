// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "AttributeModification.h"

#include "EntityAttribute.generated.h"

class AEntity;
typedef struct FEntityAttribute;

UENUM(BlueprintType)
enum class EAttributeOperationType : uint8
{
    AO_None     UMETA(DisplayName = "None"),
    AO_Add     UMETA(DisplayName = "Add"),
    AO_Remove      UMETA(DisplayName = "Remove"),
    AO_AddBuff     UMETA(DisplayName = "Add Buff"),
    AO_RemoveBuff      UMETA(DisplayName = "Remove Buff")
};
#define AO_None       EAttributeOperationType::AO_None
#define AO_Add        EAttributeOperationType::AO_Add
#define AO_Remove     EAttributeOperationType::AO_Remove
#define AO_AddBuff    EAttributeOperationType::AO_AddBuff
#define AO_RemoveBuff EAttributeOperationType::AO_RemoveBuff

DECLARE_DYNAMIC_DELEGATE_TwoParams(FModifiedDelegate, const EAttributeOperationType, Operation, const FAttributeModification&, Modification);

/**
 * Entity Attribute
 * Used as a modificable float depending on modifiers.
 * When used in entities, call SetOwner in constructor to apply Buff/Debuff modifications.
 */
USTRUCT(BlueprintType)
struct JINKCORE_API FEntityAttribute
{
    GENERATED_BODY()

    FEntityAttribute() {
        BaseValue = 0;
        Owner = nullptr;
        Guid = FGuid::NewGuid();
    }

    FEntityAttribute(float _BaseValue) {
        BaseValue = _BaseValue;
        Owner = nullptr;
        Guid = FGuid::NewGuid();
    }

    void SetOwner(AEntity* Entity) { Owner = Entity; }

    UPROPERTY(BlueprintReadWrite, Category = "Attributes")
    AEntity* Owner;

    UPROPERTY()
    FGuid Guid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float BaseValue;
	
    UPROPERTY(EditAnywhere, Category = "Attributes")
    TArray<FAttributeModification> Modifications;
    
    UPROPERTY()
    TArray<FAttributeModification> BuffModifications;

    UPROPERTY()
    FModifiedDelegate OnModified;

    void AddModification(FAttributeModification& Modification);
    void RemoveModification(FAttributeModification& Modification);
    void AddBuffModification(FAttributeModification& Modification);
    void RemoveBuffModification(FAttributeModification& Modification);

    const float GetValue() const;

    /* Assign a base value from float */

    /* Get Attribute final value */
    FORCEINLINE operator float() const
    {
        return GetValue();
    }

    //compare two modifications by guid
    FORCEINLINE bool operator==(const FEntityAttribute& Other) const
    {
        return Guid == Other.Guid;
    }
};
