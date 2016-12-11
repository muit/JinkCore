// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"

#if WITH_EDITOR
#include "UnrealEd.h"
#include "ObjectEditorUtils.h"
#endif

#include "ItemActor.h"


// Sets default values
AItemActor::AItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = SceneComponent;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
}
void AItemActor::OnConstruction(const FTransform& Transform) {
    SetItemType(ItemType);
}

UItem* AItemActor::GetItemDefaults()
{
    if (!ItemType)
        return nullptr;

    return Cast<UItem>(ItemType->GetDefaultObject());
}

void AItemActor::SetItemType(TSubclassOf<UItem> Type)
{
    ItemType = Type;
    UItem* Defaults = GetItemDefaults();
    if (Defaults) {
        Mesh->SetStaticMesh(Defaults->Mesh);
    }
    else {
        Mesh->SetStaticMesh(nullptr);
    }
}
