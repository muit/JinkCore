// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "SelectionQuery/SelQueryTest.h"

USelectionQueryGraphNode_Test::USelectionQueryGraphNode_Test(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsSubNode = true;
	bTestEnabled = true;
	bHasNamedWeight = false;
	TestWeightPct = -1.0f;
}

void USelectionQueryGraphNode_Test::InitializeInstance()
{
	USelQueryTest* TestInstance = Cast<USelQueryTest>(NodeInstance);
	if (TestInstance)
	{
		TestInstance->UpdateNodeVersion();
	}

	USelectionQueryGraphNode_Option* ParentOption = Cast<USelectionQueryGraphNode_Option>(ParentNode);
	if (ParentOption)
	{
		ParentOption->CalculateWeights();
	}
}

FText USelectionQueryGraphNode_Test::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	USelQueryTest* TestInstance = Cast<USelQueryTest>(NodeInstance);
	return TestInstance ? TestInstance->GetDescriptionTitle() : FText::GetEmpty();
}

FText USelectionQueryGraphNode_Test::GetDescription() const
{
	USelQueryTest* TestInstance = Cast<USelQueryTest>(NodeInstance);
	return TestInstance ? TestInstance->GetDescriptionDetails() : FText::GetEmpty();
}

void USelectionQueryGraphNode_Test::SetDisplayedWeight(float Pct, bool bNamed)
{
	if (TestWeightPct != Pct || bHasNamedWeight != bNamed)
	{
		Modify();
	}

	TestWeightPct = Pct;
	bHasNamedWeight = bNamed;
}
