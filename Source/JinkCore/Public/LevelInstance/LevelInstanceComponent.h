// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "LevelInstance.h"
#include "LevelInstanceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JINKCORE_API ULevelInstanceComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    ULevelInstanceComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;


    UPROPERTY(EditAnywhere, Category = "Level Instance")
    TAssetPtr<ULevelInstance> LevelInstanceAsset;

    UPROPERTY(Transient)
    ULevelInstance* LevelI_Data;

    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    bool SetLevelInstanceAsset(TAssetPtr<ULevelInstance> _LevelInstanceAsset) {
        if (_LevelInstanceAsset) {
            LevelInstanceAsset = _LevelInstanceAsset;

            LevelI_Data->RemoveLevel();

            //Create a new Level Data Instance
            LevelI_Data = nullptr;
            GetLevelInstance();
            return true;
        }
        return false;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    ULevelInstance* GetLevelInstance() {
        // if level instance data is empty and the template asset is assigned
        if (!LevelI_Data && !LevelInstanceAsset.IsNull())
        {
            ULevelInstance* Template = LevelInstanceAsset.LoadSynchronous();
            LevelI_Data = NewObject<ULevelInstance>(this, NAME_None, RF_NoFlags, Template);
        }
        return LevelI_Data;
    }


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    bool IsRegistered() {
        return LevelInstanceAsset.IsNull() ? false : GetLevelInstance()->IsRegistered();
    }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    bool IsLevelLoaded() {
        return LevelInstanceAsset.IsNull() ? false : GetLevelInstance()->IsLoaded();
    }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    bool IsLevelVisible() { 
        return LevelInstanceAsset.IsNull()? false : GetLevelInstance()->IsVisible();
    }
};
