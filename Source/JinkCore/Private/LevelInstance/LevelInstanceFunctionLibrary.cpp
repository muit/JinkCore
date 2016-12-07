// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstanceFunctionLibrary.h"

ULevelInstanceFunctionLibrary::ULevelInstanceFunctionLibrary(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool ULevelInstanceFunctionLibrary::GetTypeInfo(FLIAnchorType AnchorType, FLIAnchorTypeInfo& Info) {
    return AnchorType.GetAnchorInfo(Info);
}



