// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Item.h"
#include "ItemActor.generated.h"

UCLASS(Abstract)
class JINKCORE_API AItemActor : public AActor
{
    GENERATED_BODY()
    
public:    
    // Sets default values for this actor's properties
    AItemActor();
    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ExposeOnSpawn = true))
    TSubclassOf<UItem> ItemType;

    UFUNCTION(BlueprintCallable, Category = "Item")
    UItem* GetItemDefaults();

    UFUNCTION(BlueprintCallable, Category = "Item")
    void SetItemType(TSubclassOf<UItem> Type);
    
};
