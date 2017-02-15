// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LIModule.h"

#define LOCTEXT_NAMESPACE "LIModule" 

static ConstructorHelpers::FObjectFinderOptional<UTexture2D> ModuleTextureObject = TEXT("/Engine/EditorResources/S_Actor");

// Sets default values
ALIModule::ALIModule()
{
     // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    bLoadLevelOnPlay = true; 

    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = SceneComponent;

#if WITH_EDITORONLY_DATA
    SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
    if (SpriteComponent)
    {
        SpriteComponent->Sprite = ModuleTextureObject.Get();        // Get the sprite texture from helper class object
        SpriteComponent->SpriteInfo.DisplayName = LOCTEXT("Icon", "Icon");    // Assign sprite display name
        SpriteComponent->SetupAttachment(RootComponent);    // Attach sprite to scene component
    }
#endif // WITH_EDITORONLY_DATA

    LIComponent = CreateDefaultSubobject<ULevelInstanceComponent>(TEXT("LevelInstance"));
    LIComponent->SetupAttachment(RootComponent);
}

void ALIModule::OnConstruction( const FTransform& Transform) {
    LIComponent->bSpawnOnPlay = bLoadLevelOnPlay;
    LIComponent->SetLevelInstanceAsset(LevelInstanceAsset);
}

// Called when the game starts or when spawned
void ALIModule::BeginPlay()
{
    Super::BeginPlay();
    
}

// Called every frame
void ALIModule::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );

}

bool ALIModule::LoadLevel()
{
    return LIComponent->LoadLevel();
}

void ALIModule::UnloadLevel()
{
    LIComponent->UnloadLevel();
}

void ALIModule::AttachToAnchorByGuid(FGuid MyAnchorGUID, ULIAnchorViewerComponent * OtherAnchor)
{
    for (auto* AnchorViewer : LIComponent->AnchorViewers) {
        if (AnchorViewer->AnchorGUID == MyAnchorGUID) {
            AttachToAnchor(AnchorViewer, OtherAnchor);
            break;
        }
    }
}

void ALIModule::AttachToAnchor(ULIAnchorViewerComponent * MyAnchor, ULIAnchorViewerComponent * OtherAnchor)
{
    LIComponent->AttachToAnchor(MyAnchor, OtherAnchor);

    //Reset LIComponent transform and set it to the actor
    FTransform AttachmentTransform = LIComponent->GetComponentTransform();
    LIComponent->SetRelativeTransform(FTransform::Identity);

    SetActorTransform(AttachmentTransform);
}

#undef LOCTEXT_NAMESPACE