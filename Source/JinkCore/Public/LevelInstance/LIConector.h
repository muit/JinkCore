// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "LevelInstance.h"
#include "LevelInstanceComponent.h"
#include "LIConector.generated.h"

class ULIAnchorViewerComponent;

UCLASS(BlueprintType, meta = (DisplayName = "Level Instance Conector"))
class JINKCORE_API ALIConector : public AActor
{
    GENERATED_BODY()

    UBillboardComponent* SpriteComponent;
    // Icon sprite
    UTexture2D* SpriteTexture;

public:
    // Sets default values for this actor's properties
    ALIConector();


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Anchor|Attachment")
    ULIAnchorViewerComponent* AnchorA;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Anchor|Attachment")
    ULIAnchorViewerComponent* AnchorB;


    virtual void SetupConAttachment(ULIAnchorViewerComponent*  A, ULIAnchorViewerComponent*  B);

    UFUNCTION(BlueprintImplementableEvent, Category = "Anchor")
    void OnAttachment(ULIAnchorViewerComponent* A, ULIAnchorViewerComponent* B);

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anchor")
    bool bClosed;

    UFUNCTION(BlueprintCallable, Category = "Anchor")
    void Open();
    UFUNCTION(BlueprintCallable, Category = "Anchor")
    void Close();

    //Called at beginplay or when conector is opened/closed
    UFUNCTION(BlueprintImplementableEvent, Category = "Anchor")
    void OnOpenClose(bool _bClosed);
};
