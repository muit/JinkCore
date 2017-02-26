// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
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

    void AddModification(FAttributeModification& Modification) {
        /*if(Modification.bLinkedToOwner && Modification.Owner) {
            //Cast<AActor>(Modification.Owner)->OnDestroyed.Add
        }*/

        Modifications.Add(Modification);
    }

    void RemoveModification(FAttributeModification& Modification) {
        Modifications.Remove(Modification);
    }

    const float Calculate() const
    {
        float ActualValue = BaseValue;

        for (auto& Mod : Modifications)
        {
            Mod.Apply(*this, ActualValue);
        }

        return ActualValue;
    }

    /* Get Attribute final value */
    FORCEINLINE operator float() const
    {
        return Calculate();
    }
    /*
    void OnModOwnerDestroyed(AActor* DestroyedActor) {
        Modifications.RemoveAll([DestroyedActor] (FAttributeModification& Mod) {
            return Mod.bLinkedToOwner && Mod.Owner == DestroyedActor;
        });
    }*/
};
