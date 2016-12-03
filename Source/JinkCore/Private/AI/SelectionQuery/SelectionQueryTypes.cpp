// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AI/SelectionQuery/SQNode.h"
#include "AI/SelectionQuery/SQCompositeNode.h"
#include "AI/SelectionQuery/SelectionQueryTypes.h"

//----------------------------------------------------------------------//
// USelectionQueryTypes
//----------------------------------------------------------------------//
USelectionQueryTypes::USelectionQueryTypes(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FString USelectionQueryTypes::DescribeNodeHelper(const USQNode* Node)
{
	return Node ? FString::Printf(TEXT("%s"), *Node->GetNodeName()) : FString();
}


FString USelectionQueryTypes::GetShortTypeName(const UObject* Ob)
{
	if (Ob->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
	{
		return Ob->GetClass()->GetName().LeftChop(2);
	}

	FString TypeDesc = Ob->GetClass()->GetName();
	const int32 ShortNameIdx = TypeDesc.Find(TEXT("_"));
	if (ShortNameIdx != INDEX_NONE)
	{
		TypeDesc = TypeDesc.Mid(ShortNameIdx + 1);
	}

	return TypeDesc;
}
