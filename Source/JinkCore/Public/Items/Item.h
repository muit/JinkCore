// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Item.generated.h"

/**
*
*/
UCLASS(Abstract, BlueprintType)
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
#if WITH_EDITOR
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DesignerNotes;
#endif

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


    UFUNCTION(BlueprintNativeEvent, Category = "Item")
    void DoTest() const;
    virtual void DoTest_Implementation();
};