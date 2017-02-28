// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "EntityAttribute.h"
#include "AttributeModification.h"

#include "Buff.generated.h"

class AEntity;
class UItem;

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
    virtual void Apply(AEntity* Owner);
    virtual void Unapply();

    UFUNCTION(BlueprintImplementableEvent, Category = "Buff")
    void OnApply();
    UFUNCTION(BlueprintImplementableEvent, Category = "Buff")
    void OnUnapply();


    //~ Begin Buff Interface
    UPROPERTY(BlueprintReadOnly, Category = "Buff")
    AEntity* Entity;

    //~ Begin Buff Interface
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
    EBuffCategory Category;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
    TArray<TSubclassOf<UItem>> RestrictedItemsForPickUp;


    bool CanPickUpItem(TSubclassOf<UItem>& Type);

    UFUNCTION(BlueprintNativeEvent, Category = "Buff")
    bool _CanPickUpItem(TSubclassOf<UItem>& Type);

    UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Buff")
    void ApplyBuffModification(UPARAM(ref) FEntityAttribute& Attribute, UPARAM(ref) FAttributeModification& Mod);

    UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Buff")
    void RemoveBuffModification(UPARAM(ref) FEntityAttribute& Attribute, UPARAM(ref) FAttributeModification& Mod);
    //~ End Buff Interface

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Buff")
    FORCEINLINE bool IsApplied() const {
        return Entity != nullptr;
    }
};
