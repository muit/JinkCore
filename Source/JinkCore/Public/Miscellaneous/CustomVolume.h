// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "GameFramework/Volume.h"
#include "CustomVolume.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEntityEnterSignature, AEntity*, Entity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEntityExitSignature, AEntity*, Entity);

/**
 * 
 */
UCLASS(Blueprintable)
class JINKCORE_API ACustomVolume : public AVolume
{
    GENERATED_UCLASS_BODY()

    /**
    * Called when an instance of this class is placed (in editor) or spawned.
    * @param	Transform			The transform the actor was constructed at.
    */
    virtual void OnConstruction(const FTransform& Transform) override;

    //~ Begin UObject Interface.
#if WITH_EDITOR
    virtual void LoadedFromAnotherClass(const FName& OldClassName) override;
#endif // WITH_EDITOR
    //~ End UObject Interface.

    // PROPERTIES
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger|Gizmos")
    FColor Color;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Solid When Selected"), Category = "Trigger|Gizmos")
    bool bSolid;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
    TSubclassOf<class AEntity>  EntityFilter;


protected:
    // FUNCTIONS
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
    virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
    virtual void EntityEnter(AEntity* Entity);
    virtual void EntityExit(AEntity* Entity);


public:
    // EVENTS & DELEGATES
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EntityEnter"), Category = "Trigger")
    void ReceiveEntityEnter(AEntity* Entity);
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EntityExit"), Category="Trigger")
    void ReceiveEntityExit(AEntity* Entity);

    UPROPERTY(BlueprintAssignable, Category = "Trigger")
    FEntityEnterSignature OnEntityEnter;
    UPROPERTY(BlueprintAssignable, Category = "Trigger")
    FEntityExitSignature OnEntityExit;
};
