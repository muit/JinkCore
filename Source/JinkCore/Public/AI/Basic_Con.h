// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "AIController.h"
#include "Entity.h"
#include "Miscellaneous/Events/EventComponent.h"
#include "Basic_Con.generated.h"

class USummonList;

/**
 * 
 */
UCLASS(config = Game, BlueprintType, meta = (ShortTooltip = "A basic AI that does nothing but gives the base for more complex ones."))
class JINKCORE_API ABasic_Con : public AAIController
{
    GENERATED_BODY()

public:
    ABasic_Con(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called when an actor is possessed
    virtual void Possess(APawn* InPawn) override;

    // Called every frame
    virtual void Tick(float DeltaSeconds) override;

    virtual void CombatTick(float DeltaSeconds);

    UPROPERTY(BlueprintReadOnly, Category = "AI")
    AEntity* Me;
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
    AEntity* GetMe() { 
        if (!Me) Me = Cast<AEntity>(GetPawn());
        return Me;
    }


    /**
    * COMBAT
    */

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Follow")
    bool bFollows;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Follow")
    float MinMeleDistance;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Follow")
    float MaxMeleDistance;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Follow")
    bool bLookAtTargetWhileMele;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    AEntity* Target;

    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    FVector HomeLocation;


    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool IsMovingToTarget;
    bool StartsFollowing;


    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool AttackStart(AEntity* Victim);
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool SetTarget(AEntity* Victim); 
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StopCombat(bool reset = false);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
    bool IsValidTarget(AEntity* Entity);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
    bool IsInCombat() const { return Target != nullptr; }

    //Combat Events
    UFUNCTION(BlueprintNativeEvent, Category = "Combat")
    bool BeforeEnterCombat(AEntity* _Target);
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void EnterCombat(AEntity* _Target);
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Combat Tick"), Category = "Combat")
    void CombatTickEvent(float DeltaSeconds);
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void EndCombat(AEntity* _Target);
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void TargetChanged(AEntity* NewTarget);
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void TargetIsReached();
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void TargetIsGone();

    void JustDied_Internal(AController * InstigatedBy, AEntity* Killer);
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void JustDied(AController * InstigatedBy, AEntity* Killer);


    /**
    * NON COMBAT
    */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UEventComponent* RecoveryEvent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|NoCombat")
    bool bRecoverLiveOutOfCombat;
    //Amount of live recovered per second
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|NoCombat")
    float LiveRestorePercent;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|NoCombat|Advanced")
    bool bRecoverEvenInCombat;

    UFUNCTION()
    void StartLiveRestore();
private:
    UFUNCTION()
    void RestoreLive();


public:
    /**
     * UTIL
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity|Movement")
    bool IsAIMoving() const {
        return GetMoveStatus() == EPathFollowingStatus::Moving;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math")
    float GetDistanceToTarget() const {
        return (Target != nullptr) ? Me->GetDistanceTo(Target) : -1.0f;
    }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math")
    float GetSquaredDistanceToTarget() const {
        return (Target != nullptr) ? Me->GetSquaredDistanceTo(Target) : -1.0f;
    }


    UFUNCTION(BlueprintCallable, Category = "SummonList")
    USummonList* CreateSummonList();
};
