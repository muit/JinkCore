// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstance.h"
#include "LIConector.h"

#define LOCTEXT_NAMESPACE "LIConector" 

static ConstructorHelpers::FObjectFinderOptional<UTexture2D> ConectorTextureObject = TEXT("/Engine/EditorResources/S_KHinge");

// Sets default values
ALIConector::ALIConector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = SceneComponent;

#if WITH_EDITORONLY_DATA
    SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
    if (SpriteComponent)
    {
        SpriteComponent->Sprite = ConectorTextureObject.Get();		// Get the sprite texture from helper class object
        SpriteComponent->SpriteInfo.DisplayName = LOCTEXT("Icon", "Icon");	// Assign sprite display name
        SpriteComponent->SetupAttachment(RootComponent);	// Attach sprite to scene component
    }
#endif // WITH_EDITORONLY_DATA
}

void ALIConector::SetupConAttachment(FLIAnchor& A, FLIAnchor& B)
{
    OnAttachment(A, B);
}

// Called when the game starts or when spawned
void ALIConector::BeginPlay()
{
	Super::BeginPlay();
	
}
#undef LOCTEXT_NAMESPACE 