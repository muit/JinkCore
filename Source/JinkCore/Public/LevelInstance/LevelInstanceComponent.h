// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "LevelInstance.h"
#include "LevelInstanceComponent.generated.h"

class ULIAnchorViewerComponent;
class ALevelInstanceBounds;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLevelLoaded, ALevelInstanceBounds*, LevelInstance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelUnloaded);


UCLASS(meta=(BlueprintSpawnableComponent))
class JINKCORE_API ULevelInstanceComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance")
    TAssetPtr<ULevelInstance> LevelInstanceAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Instance", Meta = (ExposeOnSpawn = true))
    bool bSpawnOnPlay;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Instance|Debug")
    bool bDebug;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Instance|Debug")
    bool bDebugInGame;

    ULevelInstanceComponent();
    virtual void OnRegister() override;

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;
#endif //WITH_EDITOR

    virtual void DrawBounds();

    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    void SetLevelInstanceAsset(TAssetPtr<ULevelInstance> NewLevelInstanceAsset);

    //~ Begin Level Instance Interface
private:
    UPROPERTY(Transient)
    ULevelStreamingKismet* StreamingLevel;

public:
    UPROPERTY(BlueprintReadOnly, Category = "Level Instance")
    int32 InstanceId;

    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    bool LoadLevel(bool bForced = false);
    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    void SetLevelVisibility(bool NewVisibility);
    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    void UnloadLevel();

    //~ End Level Instance Interface

    //~ Begin Level Instance Interface
    UPROPERTY(BlueprintReadOnly, Category = "Level Instance")
    TArray<ULIAnchorViewerComponent*> AnchorViewers;

    UFUNCTION(BlueprintCallable, Category = "Level Instance", meta=(DisplayName = "AttachToAnchor"))
    void AttachToAnchorByGuid(FGuid MyAnchorGUID, ULIAnchorViewerComponent* OtherAnchor, bool bSpawnConector = true);

    UFUNCTION(BlueprintCallable, Category = "Level Instance", meta = (DisplayName = "AttachToAnchor"))
    void AttachToAnchor(ULIAnchorViewerComponent* MyAnchor, ULIAnchorViewerComponent* OtherAnchor, bool bSpawnConector = true);

    void UpdateAnchors();
    //~ End Level Instance Interface

    UFUNCTION()
    void OnLevelLoaded();
    UFUNCTION()
    void OnLevelUnloaded();

    UPROPERTY()
    ALevelInstanceBounds* m_LIBounds;

    UPROPERTY(BlueprintAssignable, Category = "Level Instance")
    FLevelLoaded OnLevelInstanceLoad;
    UPROPERTY(BlueprintAssignable, Category = "Level Instance")
    FLevelUnloaded OnLevelInstanceUnload;


    //Inlines
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    FORCEINLINE bool IsRegistered() const { return StreamingLevel != nullptr; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    FORCEINLINE bool IsLevelLoaded() const { return IsRegistered() && StreamingLevel->IsLevelLoaded(); }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    FORCEINLINE bool IsLevelVisible() const { return IsRegistered() && StreamingLevel->IsLevelVisible(); }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    FString GetUniqueName() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    ULevelInstance* GetLevelInstance() {
        return LevelInstanceAsset.IsNull() ? nullptr : LevelInstanceAsset.LoadSynchronous();
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    FORCEINLINE bool GetBounds(FBox& LevelBounds) {
        ULevelInstance* LIAsset = GetLevelInstance();
        if (LIAsset) {
            LevelBounds = LIAsset->Bounds;
            return true;
        }
        return false;
    }
};
