// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Miscellaneous/CustomVolume.h"
#include "LevelInstance.generated.h"

/**
*
*/
UCLASS(meta = (ShortTooltip = "A level instance is an actor that loads an streaming level in runtime."))
class JINKCORE_API ALevelInstance : public ACustomVolume
{
    GENERATED_UCLASS_BODY()

public:

    //PROPERTIES
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    TAssetPtr<UWorld> LevelAsset;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level|Settings")
    bool bRegisterOnBeginPlay;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level|Settings")
    bool bShouldBeLoaded;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level|Settings")
    bool bShouldBeVisible;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level|Settings")
    bool bShouldBlockOnLoad;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level|Settings")
    bool bInitiallyLoaded;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level|Settings")
    bool bInitiallyVisible;


    UPROPERTY()
    ULevelStreamingKismet* AssignedLevel;
    UPROPERTY(BlueprintReadOnly, Category = "LevelInstance")
    int32 InstanceId;

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable, Category = "Level")
    bool SpawnLevel();
    UFUNCTION(BlueprintCallable, Category = "Level")
    bool LoadLevel();
    UFUNCTION(BlueprintCallable, Category = "Level")
    void SetLevelVisibility(bool NewVisibility);
    UFUNCTION(BlueprintCallable, Category = "Level")
    void UnloadLevel();
    UFUNCTION(BlueprintCallable, Category = "Level")
    void RemoveLevel();


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level")
    FString GetUniqueName();

    //Helpers
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level")
    bool IsRegistered() { return AssignedLevel != nullptr; }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level")
    bool IsLevelLoaded() { return IsRegistered() && AssignedLevel->IsLevelLoaded(); }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level")
    bool IsLevelVisible() { return IsRegistered() && AssignedLevel->IsLevelVisible(); }


    //STATIC
    static int32 InstanceIdCount;
};
