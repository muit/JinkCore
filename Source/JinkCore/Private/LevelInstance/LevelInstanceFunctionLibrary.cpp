// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstanceFunctionLibrary.h"

ULevelInstanceFunctionLibrary::ULevelInstanceFunctionLibrary(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool ULevelInstanceFunctionLibrary::GetTypeInfo(UPARAM(ref) FLIAnchorType& Type, FLIAnchorTypeInfo& Info) {
    return Type.GetAnchorInfo(Info);
}

bool ULevelInstanceFunctionLibrary::Equals(UPARAM(ref) const FLIAnchorType & TypeA, UPARAM(ref) const FLIAnchorType & TypeB)
{
    return TypeA == TypeB;
}
