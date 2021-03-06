// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"

#include "Components/BoxComponent.h"
#include "LevelInstance.h"
#include "LIAnchorTargetHandle.h"

#include "LIModule.h"
#include "LevelInstanceComponent.h"

#if WITH_EDITOR
#include "ObjectEditorUtils.h"
#endif

#include "LevelInstanceBounds.h"

// Default size of the box (scale)
static const FVector DefaultLevelSize = FVector(1000.f);

ALevelInstanceBounds::ALevelInstanceBounds(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    UBoxComponent* BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent0"));
    RootComponent = BoxComponent;
    RootComponent->Mobility = EComponentMobility::Movable;
    RootComponent->RelativeScale3D = DefaultLevelSize;

    bAutoUpdateBounds = true;

    BoxComponent->bDrawOnlyIfSelected = true;
    BoxComponent->bUseAttachParentBound = false;
    BoxComponent->bUseEditorCompositing = true;
    BoxComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    BoxComponent->InitBoxExtent(FVector(0.5f, 0.5f, 0.5f));

    bCanBeDamaged = false;

#if WITH_EDITOR
    bLevelBoundsDirty = true;
    bUsingDefaultBounds = false;

    UpdateAnchors();
#endif
}

void ALevelInstanceBounds::PostLoad()
{
    Super::PostLoad();
}

FBox ALevelInstanceBounds::GetComponentsBoundingBox(bool bNonColliding) const
{
    checkf(RootComponent != nullptr, TEXT("LevelInstanceBounds actor with null root component: %s"), *GetPathNameSafe(this));

    const FTransform WorldTransform = RootComponent->GetComponentTransform();
    const FVector BoundsCenter = WorldTransform.GetLocation();
    const FVector BoundsExtent = WorldTransform.GetScale3D() * 0.5f;

    return FBox(BoundsCenter - BoundsExtent, BoundsCenter + BoundsExtent);
}

FBox ALevelInstanceBounds::CalculateLevelBounds(ULevel* InLevel)
{
    FBox LevelBounds = FBox(ForceInitToZero);

    if (InLevel)
    {
        // Iterate over all level actors
        for (int32 ActorIndex = 0; ActorIndex < InLevel->Actors.Num(); ++ActorIndex)
        {
            AActor* Actor = InLevel->Actors[ActorIndex];
            if (Actor && Actor->IsLevelBoundsRelevant())
            {
                // Sum up components bounding boxes
                FBox ActorBox = Actor->GetComponentsBoundingBox(true);
                if (ActorBox.IsValid)
                {
                    LevelBounds += ActorBox;
                }
            }
        }
    }
    return LevelBounds;
}

#if WITH_EDITOR
void ALevelInstanceBounds::PostEditUndo()
{
    Super::PostEditUndo();

    UpdateAnchors();
    MarkLevelBoundsDirty();
}

void ALevelInstanceBounds::PostEditMove(bool bFinished)
{
    Super::PostEditMove(bFinished);

    MarkLevelBoundsDirty();
}

void ALevelInstanceBounds::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    MarkLevelBoundsDirty();

    // Detect Anchors update
    static const FName NAME_LevelInstance = FName(TEXT("Level Instance"));

    if (PropertyChangedEvent.Property != NULL) {
        if (FObjectEditorUtils::GetCategoryFName(PropertyChangedEvent.MemberProperty) == NAME_LevelInstance)
        {
            FName PropName = PropertyChangedEvent.MemberProperty->GetFName();

            if (PropName == GET_MEMBER_NAME_CHECKED(ALevelInstanceBounds, Anchors)) {
                //Generate GUIDs
                for (auto Anchor : Anchors)
                {
                    if (Anchor) {
                        //Generate new GUID for each anchor
                        Anchor->GUID = FGuid::NewGuid();
                    }
                }
                UpdateAnchors();
            }
        }
    }
}

void ALevelInstanceBounds::PostRegisterAllComponents()
{
    Super::PostRegisterAllComponents();

    if (!IsTemplate())
    {
        //LevelInstance->BoundsActor = this;
        SubscribeToUpdateEvents();
    }
}

void ALevelInstanceBounds::PostUnregisterAllComponents()
{
    if (!IsTemplate())
    {
        UnsubscribeFromUpdateEvents();
    }

    Super::PostUnregisterAllComponents();
}

void ALevelInstanceBounds::Tick(float DeltaTime)
{
    if (bLevelBoundsDirty)
    {
        UpdateLevelBounds();
        bLevelBoundsDirty = false;
    }
}

TStatId ALevelInstanceBounds::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(ALevelInstanceBounds, STATGROUP_Tickables);
}

bool ALevelInstanceBounds::IsTickable() const
{
    if (GIsEditor && bAutoUpdateBounds && !IsTemplate())
    {
        UWorld* World = GetWorld();
        return (World && World->WorldType == EWorldType::Editor);
    }

    return false;
}

bool ALevelInstanceBounds::IsTickableInEditor() const
{
    return IsTickable();
}

void ALevelInstanceBounds::UpdateLevelBounds()
{
    FBox LevelBounds = CalculateLevelBounds(GetLevel());

    /** Update Level Instance Bounds in the asset  */
    if (!LevelInstance.IsNull()) {
        ULevelInstance* LevelI = LevelInstance.LoadSynchronous();
        if (LevelI) {
            LevelI->Bounds = LevelBounds;
            LevelI->MarkPackageDirty();
        }
    }

    if (LevelBounds.IsValid)
    {
        FVector LevelCenter = LevelBounds.GetCenter();
        FVector LevelSize = LevelBounds.GetSize();

        SetActorTransform(FTransform(FQuat::Identity, LevelCenter, LevelSize));
        bUsingDefaultBounds = false;
    }
    else
    {
        SetActorTransform(FTransform(FQuat::Identity, FVector::ZeroVector, DefaultLevelSize));
        bUsingDefaultBounds = true;
    }

    //BroadcastLevelBoundsUpdated();
}

void ALevelInstanceBounds::MarkLevelBoundsDirty()
{
    bLevelBoundsDirty = true;
}

bool ALevelInstanceBounds::IsUsingDefaultBounds() const
{
    return bUsingDefaultBounds;
}

void ALevelInstanceBounds::UpdateLevelBoundsImmediately()
{
    // This is used to get accurate bounds right when spawned.
    // This can't be done in PostActorCreated because the SpawnLocation interferes with the root component transform
    UpdateLevelBounds();
}

void ALevelInstanceBounds::OnLevelActorMoved(AActor* InActor)
{
    if (InActor->GetOuter() == GetOuter())
    {
        if (InActor == this)
        {
            //BroadcastLevelBoundsUpdated();
        }
        else
        {
            MarkLevelBoundsDirty();
        }
    }
}

void ALevelInstanceBounds::OnLevelActorAddedRemoved(AActor* InActor)
{
    if (InActor->GetOuter() == GetOuter())
    {
        MarkLevelBoundsDirty();
    }
}

void ALevelInstanceBounds::SubscribeToUpdateEvents()
{
    // Subscribe only in editor worlds
    if (!GetWorld()->IsGameWorld())
    {
        UnsubscribeFromUpdateEvents();

        OnLevelActorMovedDelegateHandle = GEngine->OnActorMoved().AddUObject(this, &ALevelInstanceBounds::OnLevelActorMoved);
        OnLevelActorDeletedDelegateHandle = GEngine->OnLevelActorDeleted().AddUObject(this, &ALevelInstanceBounds::OnLevelActorAddedRemoved);
        OnLevelActorAddedDelegateHandle = GEngine->OnLevelActorAdded().AddUObject(this, &ALevelInstanceBounds::OnLevelActorAddedRemoved);
    }
}

void ALevelInstanceBounds::UnsubscribeFromUpdateEvents()
{
    GEngine->OnActorMoved().Remove(OnLevelActorMovedDelegateHandle);
    GEngine->OnLevelActorDeleted().Remove(OnLevelActorDeletedDelegateHandle);
    GEngine->OnLevelActorAdded().Remove(OnLevelActorAddedDelegateHandle);
}
#endif // WITH_EDITOR


ALIAnchorTargetHandle* ALevelInstanceBounds::GetAnchorByGUID(FGuid GUID) {
    return *Anchors.FindByPredicate([GUID](const ALIAnchorTargetHandle* InAnchor)
        {
            return InAnchor->GUID == GUID;
        }
    );
}

ALIAnchorTargetHandle* ALevelInstanceBounds::GetAnchorByName(FName Name) {
    return *Anchors.FindByPredicate([Name](const ALIAnchorTargetHandle* InAnchor)
        {
            return InAnchor->Name == Name;
        }
    );
}

#if WITH_EDITOR
void ALevelInstanceBounds::UpdateAnchors()
{
    if (!LevelInstance.IsNull()) {
        ULevelInstance* LevelI = LevelInstance.LoadSynchronous();
        if (LevelI) {
            //Remove unknown anchors
            LevelI->Anchors.Empty();

            //Update or create new ones
            for (auto Anchor : Anchors)
            {
                if (Anchor) {
                    LevelI->Anchors.Add(Anchor->GetAsAnchor());
                }
            }
            LevelI->MarkPackageDirty();
        }
    }
}
#endif //WITH_EDITOR

void ALevelInstanceBounds::Internal_OnLevelInstanced(ULevelInstanceComponent* InstanceOwner)
{
    if(!IsInstanced() && InstanceOwner) {
        m_InstanceOwner = InstanceOwner;
        //Get LIModule is existing
        m_ModuleOwner = Cast<ALIModule>(InstanceOwner->GetOwner());

        OnLevelInstanced.Broadcast(m_ModuleOwner, InstanceOwner);
    }
}

void ALevelInstanceBounds::Internal_OnLevelUninstanced()
{
    if (IsInstanced()) {
        m_InstanceOwner = nullptr;
        m_ModuleOwner = nullptr;
        OnLevelUninstanced.Broadcast();
    }
}
