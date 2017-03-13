// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "LevelInstance.h"
#include "LevelInstanceComponent.h"
#include "LIModule.generated.h"

UCLASS(Blueprintable, meta = (DisplayName = "Level Instance Module"))
class JINKCORE_API ALIModule : public AActor
{
    GENERATED_BODY()

    UBillboardComponent* SpriteComponent;
    // Icon sprite
    UTexture2D* SpriteTexture;

public:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Instance", meta = (DisplayName = "Level Instance Component"))
    ULevelInstanceComponent* LIComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Instance", meta=(DisplayName = "Level Instance"))
    TAssetPtr<ULevelInstance> LevelInstanceAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Instance", Meta = (ExposeOnSpawn = true))
    bool bLoadLevelOnPlay;
    
    // Sets default values for this actor's properties
    ALIModule();
    virtual void OnConstruction(const FTransform& Transform) override;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick( float DeltaSeconds ) override;


    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    bool LoadLevel();
    UFUNCTION(BlueprintCallable, Category = "Level Instance")
    void UnloadLevel();


    UFUNCTION(BlueprintCallable, Category = "Level Instance", meta = (DisplayName = "AttachToAnchor"))
    void AttachToAnchorByGuid(FGuid MyAnchorGUID, ULIAnchorViewerComponent* OtherAnchor);
    UFUNCTION(BlueprintCallable, Category = "Level Instance", meta = (DisplayName = "AttachToAnchor"))
    virtual void AttachToAnchor(ULIAnchorViewerComponent* MyAnchor, ULIAnchorViewerComponent* OtherAnchor);
};
