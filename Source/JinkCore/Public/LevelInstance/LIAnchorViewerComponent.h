// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Components/ArrowComponent.h"
#include "LIConector.h"
#include "LIAnchorViewerComponent.generated.h"

class ALevelInstanceBounds;
struct FLIAnchor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JINKCORE_API ULIAnchorViewerComponent : public UArrowComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Anchor")
    FGuid AnchorGUID;
    UPROPERTY(VisibleAnywhere, Category = "Level Anchor")
    FLIAnchor AnchorData;

    // Sets default values for this component's properties
    ULIAnchorViewerComponent();

    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITOR

public:

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Anchor")
    FLIAnchor GetAnchor() {
        return AnchorData;
    }

    //Attachment Interface
    UPROPERTY(VisibleAnywhere, Category = "Level Anchor|Attachment")
    bool bIsAttachmentFather;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Anchor|Attachment")
    ULIAnchorViewerComponent* ConectedAttachment;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Anchor|Attachment")
    ALIConector* Conector;


    void SetupAnchorAttachment(ULIAnchorViewerComponent* OtherAnchor, bool IsFather);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Anchor|Attachment")
    bool IsAttached() { 
        return ConectedAttachment != nullptr;
    }
    UFUNCTION(BlueprintCallable, BlueprintPure,  Category = "Level Anchor|Attachment")
    bool IsAttachmentFather() {
        return IsAttached() && bIsAttachmentFather;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Level Anchor|Attachment")
        bool HaveConector() {
        return IsAttached() && Conector != nullptr;
    }
};
