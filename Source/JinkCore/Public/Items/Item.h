// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "AttributeModification.h"
#include "Item.generated.h"

class UBuff;
class AEntity;

/**
*
*/
UCLASS(Abstract, Blueprintable)
class JINKCORE_API UItem : public UObject
{
    GENERATED_BODY()

public:
    UItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    //~ Begin Item Interface
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture* Icon;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UStaticMesh* Mesh;

#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DesignerNotes;
#endif
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
    bool bUnique;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
    bool bUsable;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
    bool bStackable;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (DisplayName = "Damage"))
    FAttributeModification DamageMod;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (DisplayName = "Live"))
    FAttributeModification LiveMod;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (DisplayName = "Movement"))
    FAttributeModification MovementMod;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (DisplayName = "Fire Rate"))
    FAttributeModification FireRateMod;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (DisplayName = "Bullet Speed"))
    FAttributeModification BulletSpeedMod;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
    TArray<TSubclassOf<UBuff>> BuffsApplied;
    UPROPERTY(Transient)
    TArray<UBuff*> BuffsAppliedObjects;

    UPROPERTY(BlueprintReadWrite, Category = "Properties")
    int Count;
    //~ End Item Interface

protected:
    UPROPERTY(BlueprintReadOnly)
    AEntity* Holder;

public:

    void PickUp(AEntity* Entity);
    void Drop();

protected:

    UFUNCTION(BlueprintNativeEvent, Category = "Item")
    void OnPickUp(AEntity* Entity);

    UFUNCTION(BlueprintNativeEvent, Category = "Item")
    void OnDrop();


    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
    void NotifyFired(bool& canFire);

    UFUNCTION(BlueprintNativeEvent, Category = "Item")
    void HolderJustDied(AController * InstigatedBy, AEntity * Killer);

public:

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Item")
    bool IsPickedUp() { return Holder != nullptr; }
    

    virtual UWorld* GetWorld() const override;
};