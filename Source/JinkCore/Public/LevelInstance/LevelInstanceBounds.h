// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Tickable.h"
#include "Engine/LevelBounds.h"
#include "LevelInstanceBounds.generated.h"

class ULevelInstance;
class ALIAnchorTargetHandle;

class ALIModule;
class ULevelInstanceComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLevelInstanced, ALIModule*, Module, ULevelInstanceComponent*, InstanceOwner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelUninstanced);

/**
*
* Defines level bounds
* Updates bounding box automatically based on actors transformation changes or holds fixed user defined bounding box
* Uses only actors where AActor::IsLevelBoundsRelevant() == true
*/
UCLASS(hidecategories = (Advanced, Collision, Display, Rendering, Physics, Input), showcategories = ("Input|MouseInput", "Input|TouchInput"), MinimalAPI)
class ALevelInstanceBounds
    : public AActor
    , public FEditorTickableLevelBounds
{
    GENERATED_UCLASS_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Instance")
    TAssetPtr<ULevelInstance> LevelInstance;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Instance")
    TArray<ALIAnchorTargetHandle*> Anchors;

    //~ Begin UObject Interface
    virtual void PostLoad() override;
    //~ End UObject Interface


    //~ Begin AActor Interface.
    virtual FBox GetComponentsBoundingBox(bool bNonColliding = false) const override;
    virtual bool IsLevelBoundsRelevant() const override { return false; }
    //~ End AActor Interface.


    //~ Begin ALevelBounds Interface
    /** Whether to automatically update actor bounds based on all relevant actors bounds belonging to the same level */
    UPROPERTY(EditAnywhere, Category = LevelBounds)
    bool bAutoUpdateBounds;

    /** @return Bounding box which includes all relevant actors bounding boxes belonging to specified level */
    JINKCORE_API static FBox CalculateLevelBounds(ULevel* InLevel);

#if WITH_EDITOR
    virtual void PostEditUndo() override;
    virtual void PostEditMove(bool bFinished) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostRegisterAllComponents() override;
    virtual void PostUnregisterAllComponents() override;

    /** Marks level bounds as dirty so they will be recalculated on next tick */
    void MarkLevelBoundsDirty();

    /** @return True if there were no actors contributing to bounds and we are currently using the default bounds */
    JINKCORE_API bool IsUsingDefaultBounds() const;

    /** Update level bounds immediately so the bounds are accurate when returning. Use only when needed because updating the bounds is slow */
    JINKCORE_API void UpdateLevelBoundsImmediately();

private:
    /** FTickableGameObject interface */
    virtual void Tick(float DeltaTime) override;
    virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
    virtual TStatId GetStatId() const override;
    virtual bool IsTickable() const override;
    virtual bool IsTickableInEditor() const override;
    

    /** Updates this actor bounding box by summing all level actors bounding boxes  */
    void UpdateLevelBounds();

    /** Called whenever any actor moved  */
    void OnLevelActorMoved(AActor* InActor);

    /** Called whenever any actor added or removed  */
    void OnLevelActorAddedRemoved(AActor* InActor);

    /** Subscribes for actors transformation events */
    void SubscribeToUpdateEvents();

    /** Unsubscribes from actors transformation events */
    void UnsubscribeFromUpdateEvents();

    /** Whether currently level bounds is dirty and needs to be updated  */
    bool bLevelBoundsDirty;

    /** True when there are no actors contributing to the bounds and we are currently using the default bounds instead */
    bool bUsingDefaultBounds;

    /** Handles to various registered delegates */
    FDelegateHandle OnLevelActorMovedDelegateHandle;
    FDelegateHandle OnLevelActorDeletedDelegateHandle;
    FDelegateHandle OnLevelActorAddedDelegateHandle;
#endif
    //~ End ALevelBounds Interface


    //~ Begin ALevelInstanceBounds Interface.
public:
    TSharedPtr<ALIAnchorTargetHandle> GetAnchorByGUID(FGuid GUID);
    TSharedPtr<ALIAnchorTargetHandle> GetAnchorByName(FName Name);

#if WITH_EDITOR
    /** Updates anchors in the level instance asset  */
    void UpdateAnchors();
#endif //WITH_EDITOR

    void Internal_OnLevelInstanced(ULevelInstanceComponent* InstanceOwner);
    void Internal_OnLevelUninstanced();

    UPROPERTY(BlueprintReadOnly, Category = "Level Instance")
    ULevelInstanceComponent* m_InstanceOwner;
    UPROPERTY(BlueprintReadOnly, Category = "Level Instance")
    ALIModule* m_ModuleOwner;

    UPROPERTY(BlueprintAssignable, Category = "Level Instance")
    FLevelInstanced OnLevelInstanced;
    UPROPERTY(BlueprintAssignable, Category = "Level Instance")
    FLevelUninstanced OnLevelUninstanced;

    /** Is this level instanced? */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Level Instance")
    FORCEINLINE bool IsInstanced() { return m_InstanceOwner != nullptr; }
    /** Is this instance contained in a LIModule? */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Instance")
    FORCEINLINE bool HasAModule() { return IsInstanced() && m_ModuleOwner != nullptr; }

    //~ End ALevelInstanceBounds Interface.
};