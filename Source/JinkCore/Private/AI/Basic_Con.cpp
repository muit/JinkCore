// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Basic_Con.h"


// Sets default values
ABasic_Con::ABasic_Con(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    bAttachToPawn = true;

    bFollows = true;
    MinMeleDistance = 100.0f;
    MaxMeleDistance = MinMeleDistance + 100.0f;
    IsMovingToTarget = true;
    StartsFollowing = true;
    bLookAtTargetWhileMele = true;

    bRecoverLiveOutOfCombat = true;
    LiveRestorePercent = 13;
    bRecoverEvenInCombat = false;

    RecoveryEvent = CreateDefaultSubobject<UEventComponent>(TEXT("LiveRecoveryEvent"));
    RecoveryEvent->bLooping = true;
    RecoveryEvent->DefaultLength = 0.5f;
    //Bind restore function
    RecoveryEvent->Execute.AddDynamic(this, &ABasic_Con::RestoreLive);
    AddOwnedComponent(RecoveryEvent);
}

// Called when the game starts or when spawned
void ABasic_Con::BeginPlay()
{
    Super::BeginPlay();
}

void ABasic_Con::Possess(APawn* InPawn)
{
    Super::Possess(InPawn);
    //Setup Me
    Me = GetMe();

    HomeLocation = Me->GetActorLocation();

    if (bLookAtTargetWhileMele) {
        Me->CharacterMovement->bUseControllerDesiredRotation = true;
    }
}

// Called every frame
void ABasic_Con::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!GetMe())
        return;

    if (!GetMe()->IsAlive())
        return;

    if (IsInCombat()) {
        //Call combat ticks
        CombatTick(DeltaTime);
        CombatTickEvent(DeltaTime);
    }
    else 
    {
        //No combat tick
        if (GetMe()->Live < GetMe()->MaxLive) {
            StartLiveRestore();
        }
    }
}

void ABasic_Con::CombatTick(float DeltaSeconds)
{
    if (!Target->IsAlive()) {
        StopCombat();
        return;
    }

    GetMe()->CharacterMovement->bUseControllerDesiredRotation = false;

    if (bFollows) {
        if (!IsAIMoving()) {
            if (GetSquaredDistanceToTarget() < FMath::Pow(MaxMeleDistance, 2))
            {
                if (IsMovingToTarget || StartsFollowing) {
                    IsMovingToTarget = false;
                    TargetIsReached();
                }
                if (bLookAtTargetWhileMele) {
                    GetMe()->RotateTowardsActor(Target);
                }
                //Mele Attack

            }
            else
            {
                IsMovingToTarget = true;
                TargetIsGone();
                //If not moving, follow the target
                MoveToActor(Target, MinMeleDistance);
            }
            StartsFollowing = false;
        }
    }
    //Stop if not following
    else {
        StartsFollowing = true;
        if (IsAIMoving() && IsMovingToTarget) {
            StopMovement();
        }
    }
}

bool ABasic_Con::AttackStart(AEntity * Victim)
{
    if (!GetMe()) {
        UE_LOG(LogJinkCore, Warning, TEXT("JinkCore: Me is NULL, can't start combat."));
        return false;
    }

    if (!GetMe()->IsAlive())
        return false;
    
    //Stop Live Recovery
    RecoveryEvent->Reset();

    if (!IsInCombat()) {
        if (IsValidTarget(Victim) && Victim != Target) {
            //Call Before Combat Start and use it as a validation
            if (BeforeEnterCombat(Victim)) {
                //Set Target
                Target = Victim;
                UE_LOG(LogJinkCore, Log, TEXT("JinkCore AI: %s started combat with '%s'"), *GetMe()->GetName(), *Victim->GetName());
                EnterCombat(Target);
                return true;
            }
        }
    }
    else {
        return SetTarget(Victim);
    }

    return false;
}

bool ABasic_Con::SetTarget(AEntity * Victim) {
    if (IsValidTarget(Victim) && Victim != Target) {
        Target = Victim;
        UE_LOG(LogJinkCore, Log, TEXT("JinkCore AI: %s changed target to '%s'"), *GetMe()->GetName(), *Victim->GetName());
        TargetChanged(Target);
        return true;
    }
    return false;
}

void ABasic_Con::StopCombat(bool reset)
{
    if (IsInCombat()) {
        AEntity* OldTarget = Target;
        Target = nullptr;
        MoveToLocation(HomeLocation);

        if (reset) {
            GetMe()->Live = GetMe()->MaxLive;
        }
        EndCombat(OldTarget);
    }
}

bool ABasic_Con::IsValidTarget(AEntity * Entity)
{
    if (Entity != nullptr) {
        return Entity != GetMe() && GetMe()->IsHostileTo(Entity);
    }
    return false;
}

void ABasic_Con::JustDied_Internal(AController * InstigatedBy, AEntity * Killer)
{
    GetMe()->GetCharacterMovement()->StopMovementImmediately();
    Target = nullptr;
    JustDied(InstigatedBy, Killer);
}

bool ABasic_Con::BeforeEnterCombat_Implementation(AEntity* Entity) {
    return true;
}

void ABasic_Con::StartLiveRestore()
{
    if (bRecoverLiveOutOfCombat) {
        if ((!IsInCombat() || bRecoverEvenInCombat) && GetMe()->IsAlive()) {
            RecoveryEvent->Start();
        }
    }
}

void ABasic_Con::RestoreLive()
{
    const float MaxLive = GetMe()->MaxLive;

    if (GetMe()->Live >= MaxLive) {
        RecoveryEvent->Reset();
        return;
    }

    //Calculate live increment by percent & event length
    const float NewLive = GetMe()->Live + MaxLive*(LiveRestorePercent/100) * RecoveryEvent->GetLength();

    GetMe()->Live = FMath::Clamp(NewLive, 0.0f, MaxLive);
}
