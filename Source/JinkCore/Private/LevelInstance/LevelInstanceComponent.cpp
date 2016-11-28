// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"

#include "LIAnchorViewerComponent.h"

#if WITH_EDITOR
#include "UnrealEd.h"
#include "ObjectEditorUtils.h"
#include "WorldBrowserModule.h"
#include "Private/WorldBrowserPrivatePCH.h"
#endif

#include "LevelInstanceComponent.h"


ULevelInstanceComponent::ULevelInstanceComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
    bTickInEditor = true;
    bSpawnOnPlay = true;
    bDebug = true;
    bDebugInGame = false;

    InstanceId = -1;
    StreamingLevel = nullptr;
}

void ULevelInstanceComponent::OnRegister()
{
    Super::OnRegister();
    UpdateAnchors();
}

void ULevelInstanceComponent::BeginPlay()
{
    Super::BeginPlay();

    // TEMPORAL: Spawn Level on Begin Play
    if (bSpawnOnPlay) {
        SpawnLevel();
    }
}

void ULevelInstanceComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    if (GetWorld()->WorldType == EWorldType::Type::Editor ||
        GetWorld()->WorldType == EWorldType::Type::None)
    {
        //Editor Tick
        if (bDebug) {
            DrawBounds();
        }
        return;
    }

    //In Game Tick
    if (bDebugInGame) {
        DrawBounds();

    }
}

void ULevelInstanceComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    UnloadLevel();
    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

#if WITH_EDITOR
void ULevelInstanceComponent::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
    static const FName NAME_LevelInstance = FName(TEXT("Level Instance"));
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.Property != NULL) {
        if (FObjectEditorUtils::GetCategoryFName(PropertyChangedEvent.Property) == NAME_LevelInstance)
        {
            FName PropName = PropertyChangedEvent.Property->GetFName();

            if (PropName == GET_MEMBER_NAME_CHECKED(ULevelInstanceComponent, LevelInstanceAsset)) {
#if WITH_EDITORONLY_DATA
               UpdateAnchors();
#endif // WITH_EDITORONLY_DATA
            } 
            else if (PropName == GET_MEMBER_NAME_CHECKED(ULevelInstanceComponent, bDebugInGame)) {
                for (auto* Anchor : AnchorViewers)
                {
                    Anchor->bHiddenInGame = !bDebugInGame;
                }
            }
        }
    }
}
#endif //WITH_EDITOR

void ULevelInstanceComponent::DrawBounds() {
    if (!LevelInstanceAsset.IsNull()) {
        //get the last level instance bounds box
        const FBox LevelBounds = GetLevelInstance()->Bounds;
        const FVector Center = GetComponentLocation() + LevelBounds.GetCenter();
        const FVector Extent = LevelBounds.GetExtent() * GetComponentScale();

        DrawDebugBox(GetWorld(), Center, Extent, GetComponentQuat(), FColor::Purple);
    }
}

void ULevelInstanceComponent::SetLevelInstanceAsset(TAssetPtr<ULevelInstance> NewLevelInstanceAsset) {
    if (!NewLevelInstanceAsset.IsNull()) {
        if (LevelInstanceAsset != NewLevelInstanceAsset) {
            //Remove Last Level
            UnloadLevel();
        }

        LevelInstanceAsset = NewLevelInstanceAsset;
        UpdateAnchors();
    }
}

ULevelInstance* ULevelInstanceComponent::GetLevelInstance() {
    return LevelInstanceAsset.IsNull()? nullptr : LevelInstanceAsset.LoadSynchronous();
}


//~ Begin Level Instance Interface

bool ULevelInstanceComponent::SpawnLevel(bool bForced)
{
    UE_LOG(LogJinkCore, Display, TEXT("LevelInstance: Spawning"));

    if (LevelInstanceAsset.IsNull()) {
        UE_LOG(LogJinkCore, Error, TEXT("LevelInstance: LevelInstanceAsset is empty"));
        return false;
    }
    if(IsRegistered() && !bForced) {
        UE_LOG(LogJinkCore, Warning, TEXT("LevelInstance: Already registered"));
        return false;
    }

    //May need to load LevelInstanceAsset
    LevelInstanceAsset.LoadSynchronous();

    const TAssetPtr<UWorld> Level = LevelInstanceAsset->InstancedLevel;

    if (Level.IsNull()) {
        UE_LOG(LogJinkCore, Error, TEXT("LevelInstance: Instanced Level is empty"));
        return false;
    }

    UWorld* const World = GetWorld();
    if (!World) {
        UE_LOG(LogJinkCore, Error, TEXT("LevelInstance: Couldn't get World while spawning the level"));
        return false;
    }

    // Check whether requested map exists, this could be very slow if LevelName is a short package name
    FString LevelName = Level.GetLongPackageName();
    FString LongPackageName = FPackageName::FilenameToLongPackageName(LevelName);


    // Create Unique Name for sub-level package
    const FString ShortPackageName = FPackageName::GetShortName(LongPackageName);
    const FString PackagePath = FPackageName::GetLongPackagePath(LongPackageName);
    FString UniqueLevelPackageName = PackagePath + TEXT("/") + World->StreamingLevelsPrefix + ShortPackageName;
    UniqueLevelPackageName += TEXT("_LevelInstance_") + FString::FromInt(LevelInstanceAsset->InstanceIdCount);
    // Increment Id counter
    InstanceId = LevelInstanceAsset->InstanceIdCount;
    LevelInstanceAsset->InstanceIdCount++;

    // Setup streaming level object that will load specified map
    ULevelStreamingKismet* NewStreamingLevel = NewObject<ULevelStreamingKismet>(World, ULevelStreamingKismet::StaticClass(), NAME_None, RF_Transient, NULL);
    NewStreamingLevel->SetWorldAssetByPackageName(FName(*UniqueLevelPackageName));
    NewStreamingLevel->LevelColor = FColor::MakeRandomColor();
    NewStreamingLevel->bShouldBeLoaded = LevelInstanceAsset->bShouldBeLoaded;
    NewStreamingLevel->bShouldBeVisible = LevelInstanceAsset->bShouldBeVisible;
    NewStreamingLevel->bShouldBlockOnLoad = LevelInstanceAsset->bShouldBlockOnLoad;
    NewStreamingLevel->bInitiallyLoaded = LevelInstanceAsset->bInitiallyLoaded;
    NewStreamingLevel->bInitiallyVisible = LevelInstanceAsset->bInitiallyVisible;

    // Transform
    NewStreamingLevel->LevelTransform = GetComponentTransform();
    // Map to Load
    NewStreamingLevel->PackageNameToLoad = FName(*LongPackageName);

    // Add the new level to world.
    World->StreamingLevels.Add(NewStreamingLevel);

    //Save level reference
    StreamingLevel = NewStreamingLevel;

    UE_LOG(LogJinkCore, Display, TEXT("LevelInstance: Spawned Succesfully"));
    return true;
}

bool ULevelInstanceComponent::LoadLevel()
{
    if (!IsRegistered())
        return false;
    StreamingLevel->bShouldBeLoaded = true;
    return true;
}

void ULevelInstanceComponent::SetLevelVisibility(bool NewVisibility)
{
    if (IsRegistered()) {
        StreamingLevel->bShouldBeVisible = NewVisibility;
    }
}

void ULevelInstanceComponent::UnloadLevel()
{
    if (IsRegistered()) {
        StreamingLevel->bShouldBeLoaded = false;
    }
}

FString ULevelInstanceComponent::GetUniqueName()
{
    if (!IsRegistered()) {
        return TEXT("None");
    }
    return TEXT("");
}
//~ End Level Instance Interface


void ULevelInstanceComponent::AttachToAnchorByGuid(FGuid MyAnchorGUID, ULIAnchorViewerComponent * OtherAnchor)
{
    for (auto* AnchorViewer : AnchorViewers) {
        if (AnchorViewer->AnchorGUID == MyAnchorGUID) {
            AttachToAnchor(AnchorViewer, OtherAnchor);
            break;
        }
    }
}

void ULevelInstanceComponent::AttachToAnchor(ULIAnchorViewerComponent * MyAnchor, ULIAnchorViewerComponent * OtherAnchor)
{
    if (!MyAnchor || !OtherAnchor) {
        UE_LOG(LogJinkCore, Warning, TEXT("LevelInstance: Can't attach with an null anchor."));
        return;
    }

    if (!AnchorViewers.Contains(MyAnchor) || AnchorViewers.Contains(OtherAnchor)) {
        UE_LOG(LogJinkCore, Warning, TEXT("LevelInstance: Can't attach with an invalid anchor."));
        return;
    }

    //Attach Anchor To Root
    MyAnchor->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
    //Attach LIComponent to this anchor
    this->AttachToComponent(MyAnchor, FAttachmentTransformRules::KeepWorldTransform);

    //Set Anchor Location & Rotation to the other anchor
    FTransform Transform = OtherAnchor->GetComponentTransform();
    FVector AttachDirection = -Transform.GetRotation().GetForwardVector();
    Transform.SetRotation(AttachDirection.ToOrientationRotator().Quaternion());

    MyAnchor->SetWorldTransform(Transform);

    //Attach LIComponent To Root
    this->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
    //Attach Anchor To LIComponent
    MyAnchor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
}

//~ Begin Anchors Interface
void ULevelInstanceComponent::UpdateAnchors()
{
    TArray<USceneComponent*> Childrens;
    GetChildrenComponents(true, Childrens);
    if (Childrens.Num() > 0) {
        //Remove previous anchor viewers
        for (auto* Comp : Childrens)
        {
            if (Comp->StaticClass() == ULIAnchorViewerComponent::StaticClass())
                Comp->DestroyComponent();
        }
    }
    AnchorViewers.Empty();

    if (LevelInstanceAsset.IsNull())
        return;

    ULevelInstance* LevelI = LevelInstanceAsset.LoadSynchronous();


    for (auto& Anchor : LevelI->Anchors)
    {
        //Create a new viewer for each anchor
        ULIAnchorViewerComponent* AnchorViewer = NewObject<ULIAnchorViewerComponent>(GetOwner(), ULIAnchorViewerComponent::StaticClass(), Anchor.Name);
        if (AnchorViewer)
        {
            UE_LOG(LogJinkCore, Display, TEXT("LevelInstance: Creating Anchor Viewer"));

            AnchorViewer->RegisterComponent();

            FLIAnchorTypeInfo TypeInfo;
            if (Anchor.Type.GetAnchorInfo(TypeInfo)) {
                AnchorViewer->SetArrowColor_New(TypeInfo.Color);
            }

            AnchorViewer->AnchorGUID = Anchor.GUID;
            AnchorViewer->bHiddenInGame = !bDebugInGame;
            //Move to the local space anchor position
            AnchorViewer->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
            AnchorViewer->SetRelativeTransform(Anchor.Transform);
            AnchorViewers.Add(AnchorViewer);
        }
    }
}
//~ End Anchors Interface
