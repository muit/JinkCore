// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Item.generated.h"

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DesignerNotes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
    bool bUnique;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
    bool bUsable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
    float DamageIncrement;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
    float LiveIncrement;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (ClampMin = "0", UIMin = "0.5", UIMax = "1.5"))
    float MovementSpeedCof;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (ClampMin = "0", UIMin = "0.5", UIMax = "1.5"))
    float FireRateCof;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties", meta = (ClampMin = "0", UIMin = "0.5", UIMax = "1.5"))
    float BulletSpeedCof;
    //~ End Item Interface

    void SetHolder(AEntity* Entity);
private:
    UPROPERTY()
    AEntity* Holder;
public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Item")
    AEntity* GetHolderEntity();


    UFUNCTION(BlueprintNativeEvent, Category = "Item")
    void HolderJustDied(AEntity* Entity, AController * InstigatedBy, AEntity * Killer);

    UFUNCTION(BlueprintNativeEvent, Category = "Item")
    void ApplyEntityModifications(AEntity* Entity);
    UFUNCTION(BlueprintNativeEvent, Category = "Item")
    void UndoEntityModifications(AEntity* Entity);


    FORCEINLINE static UItem* GetObject(TSubclassOf<UItem> Type, AEntity* Holder = nullptr) {
        if (Type) {
            UItem* Item = Cast<UItem>(Type->GetDefaultObject());
            if (Item) {
                Item->SetHolder(Holder);
            }
            return Item;
        }
        return nullptr;
    }

    template<class ItemType>
    FORCEINLINE static ItemType* GetObject(AEntity* Holder = nullptr) {
        TSubclassOf<UItem> Type = ItemType::StaticClass();
        if (Type) {
            UItem* Item = Cast<ItemType>(Type->GetDefaultObject());
            if (Item) {
                Item->SetHolder(Holder);
            }
            return Item;
        }
        return nullptr;
    }
};