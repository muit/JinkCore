// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AttributeModification.h"

#include "EntityAttribute.generated.h"

class AEntity;

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
        Guid = FGuid::NewGuid();
    }

    FEntityAttribute(float _BaseValue) {
        BaseValue = _BaseValue;
        Guid = FGuid::NewGuid();
    }

    FEntityAttribute(AEntity* _Owner, float _BaseValue = 0) {
        Owner = _Owner;
        BaseValue = _BaseValue;
        Guid = FGuid::NewGuid();
    }

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGuid Guid;

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    AEntity* Owner;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float BaseValue;
	
    UPROPERTY(EditAnywhere, Category = "Attributes")
    TArray<FAttributeModification> Modifications;

    UPROPERTY()
    TArray<FAttributeModification> BuffModifications;

    void AddModification(FAttributeModification& Modification) {
        Modifications.Add(Modification);
    }

    void RemoveModification(FAttributeModification& Modification) {
        Modifications.Remove(Modification);
    }

    void AddBuffModification(FAttributeModification& Modification) {
        BuffModifications.Add(Modification);
    }
    void RemoveBuffModification(FAttributeModification& Modification) {
        BuffModifications.Remove(Modification);
    }

    const float Calculate() const;

    /* Assign a base value from float */
    FORCEINLINE FEntityAttribute operator= (const float& base) const
    {
        return FEntityAttribute(base);
    }

    /* Get Attribute final value */
    FORCEINLINE operator float() const
    {
        return Calculate();
    }

    //compare two modifications by guid
    FORCEINLINE bool operator==(const FEntityAttribute& Other) const
    {
        return Guid == Other.Guid;
    }

    /*
    void OnModOwnerDestroyed(AActor* DestroyedActor) {
        Modifications.RemoveAll([DestroyedActor] (FAttributeModification& Mod) {
            return Mod.bLinkedToOwner && Mod.Owner == DestroyedActor;
        });
    }*/
};
