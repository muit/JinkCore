// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LIAnchorType.h"
#include "LevelInstance.h"
#include "LevelInstanceBounds.h"

#if WITH_EDITOR
#include "ObjectEditorUtils.h"
#endif

#include "LIAnchorTargetHandle.h"

ALIAnchorTargetHandle::ALIAnchorTargetHandle(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
#if WITH_EDITOR
    UArrowComponent* Arrow = GetArrowComponent();
    if (Arrow) {
        Arrow->SetVisibility(true);
        Arrow->ArrowSize = 3;
        Arrow->bIsScreenSizeScaled = true;
        Arrow->ScreenSize = 0.0025f;
    }

    UpdateLIBounds();
#endif //WITH_EDITOR

    Name = "Conector";
}

FLIAnchor ALIAnchorTargetHandle::GetAsAnchor()
{
    FLIAnchor Anchor = FLIAnchor();
    Anchor.Name = Name;
    Anchor.Type = Type;
    Anchor.GUID = GUID;
    Anchor.Transform = GetActorTransform();
    return Anchor;
}

#if WITH_EDITOR
void ALIAnchorTargetHandle::PostEditUndo()
{
    Super::PostEditUndo();

    UpdateLIBounds();
}

void ALIAnchorTargetHandle::PostEditMove(bool bFinished)
{
    Super::PostEditMove(bFinished);

    UpdateLIBounds();
}

void ALIAnchorTargetHandle::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // Detect Anchors update
    static const FName NAME_LevelInstance = FName(TEXT("Anchor"));

    if (PropertyChangedEvent.Property != NULL) {
        if (FObjectEditorUtils::GetCategoryFName(PropertyChangedEvent.MemberProperty) == NAME_LevelInstance)
        {
            UpdateLIBounds();
        }
    }
}

//Force the level instance to update the anchors
void ALIAnchorTargetHandle::UpdateLIBounds()
{
    FLIAnchorTypeInfo TypeInfo;
    if(Type.GetAnchorInfo(TypeInfo)) {
        UArrowComponent* Arrow = GetArrowComponent();
        if (Arrow) {
            Arrow->ArrowColor = TypeInfo.Color;
            Arrow->MarkPackageDirty();
        }
    }

    //If no bounds assigned, try to find ones
    if (!BoundsOwner) {
        TArray<AActor*> BoundsActors;
        UGameplayStatics::GetAllActorsOfClass(this, ALevelInstanceBounds::StaticClass(), BoundsActors);

        if (BoundsActors.Num() >= 1) {
            if (ALevelInstanceBounds* LIBounds = Cast<ALevelInstanceBounds>(BoundsActors[0])) {
                BoundsOwner = LIBounds;
            }
        }
    }

    if (BoundsOwner) {
        BoundsOwner->UpdateAnchors();
    }
}
#endif //WITH_EDITOR
