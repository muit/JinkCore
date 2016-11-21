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
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void OnComponentDestroyed (bool bDestroyingHierarchy) override;
#if WITH_EDITOR
    virtual void OnRegister() override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;
    void UpdateLevelInEditor();
#endif //WITH_EDITOR


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance")
    TAssetPtr<ULevelInstance> LevelInstanceAsset;
#if WITH_EDITOR
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Instance")
    bool ViewBounds;
#endif //WITH_EDITOR

    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    void SetLevelInstanceAsset(TAssetPtr<ULevelInstance> NewLevelInstanceAsset);
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    ULevelInstance* GetLevelInstance();

    //~ Begin Level Instance Interface
private:
    UPROPERTY(Transient)
    ULevelStreamingKismet* StreamingLevel;

public:
    UPROPERTY(BlueprintReadOnly, Category = "Level Instance")
    int32 InstanceId;

    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    bool SpawnLevel();
    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    bool LoadLevel();
    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    void SetLevelVisibility(bool NewVisibility);
    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    void UnloadLevel();
    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    void RemoveLevel();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    FString GetUniqueName();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    bool IsRegistered() { return StreamingLevel != nullptr; }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    bool IsLevelLoaded() { return IsRegistered() && StreamingLevel->IsLevelLoaded(); }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    bool IsLevelVisible() { return IsRegistered() && StreamingLevel->IsLevelVisible(); }
    //~ End Level Instance Interface


    /*
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
    }*/
};
