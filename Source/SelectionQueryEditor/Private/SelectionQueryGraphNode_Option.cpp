// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "ScopedTransaction.h"
#include "SGraphEditorActionMenuAI.h"
#include "SelectionQuery/SelQueryTest.h"
#include "SelectionQuery/SelQueryOption.h"
#include "SelectionQuery/Generators/SelQueryGenerator_Composite.h"

#define LOCTEXT_NAMESPACE "SelectionQueryEditor"

USelectionQueryGraphNode_Option::USelectionQueryGraphNode_Option(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USelectionQueryGraphNode_Option::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Transition"), TEXT(""), NULL, false, false, TEXT("Out"));
}

void USelectionQueryGraphNode_Option::PostPlacedNewNode()
{
	UClass* NodeClass = ClassData.GetClass(true);
	if (NodeClass)
	{
		UEdGraph* MyGraph = GetGraph();
		UObject* GraphOwner = MyGraph ? MyGraph->GetOuter() : nullptr;
		if (GraphOwner)
		{
			USelQueryOption* QueryOption = NewObject<USelQueryOption>(GraphOwner);
			QueryOption->Generator = NewObject<USelQueryGenerator>(GraphOwner, NodeClass);
			QueryOption->Generator->UpdateNodeVersion();

			QueryOption->SetFlags(RF_Transactional);
			QueryOption->Generator->SetFlags(RF_Transactional);

			NodeInstance = QueryOption;
			InitializeInstance();
		}
	}
}

void USelectionQueryGraphNode_Option::ResetNodeOwner()
{
	Super::ResetNodeOwner();

	USelQueryOption* OptionInstance = Cast<USelQueryOption>(NodeInstance);
	if (OptionInstance && OptionInstance->Generator)
	{
		UObject* GraphOwner = GetGraph() ? GetGraph()->GetOuter() : nullptr;
		OptionInstance->Generator->Rename(NULL, GraphOwner, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

void USelectionQueryGraphNode_Option::PrepareForCopying()
{
	Super::PrepareForCopying();

	USelQueryOption* OptionInstance = Cast<USelQueryOption>(NodeInstance);
	if (OptionInstance && OptionInstance->Generator)
	{
		// Temporarily take ownership of the node instance, so that it is not deleted when cutting
		OptionInstance->Generator->Rename(NULL, this, REN_DontCreateRedirectors | REN_DoNotDirty );
	}
}

void USelectionQueryGraphNode_Option::UpdateNodeClassData()
{
	USelQueryOption* OptionInstance = Cast<USelQueryOption>(NodeInstance);
	if (OptionInstance && OptionInstance->Generator)
	{
		UpdateNodeClassDataFrom(OptionInstance->Generator->GetClass(), ClassData);
		ErrorMessage = ClassData.GetDeprecatedMessage();
	}
}

FText USelectionQueryGraphNode_Option::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	USelQueryOption* OptionInstance = Cast<USelQueryOption>(NodeInstance);
	return OptionInstance ? OptionInstance->GetDescriptionTitle() : FText::GetEmpty();
}

FText USelectionQueryGraphNode_Option::GetDescription() const
{
	USelQueryOption* OptionInstance = Cast<USelQueryOption>(NodeInstance);
	return OptionInstance ? OptionInstance->GetDescriptionDetails() : FText::GetEmpty();
}

void USelectionQueryGraphNode_Option::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	Context.MenuBuilder->AddSubMenu(
		LOCTEXT("AddTest", "Add Test..." ),
		LOCTEXT("AddTestTooltip", "Adds new test to generator" ),
		FNewMenuDelegate::CreateUObject( this, &USelectionQueryGraphNode_Option::CreateAddTestSubMenu,(UEdGraph*)Context.Graph) 
		);
}

void USelectionQueryGraphNode_Option::CreateAddTestSubMenu(class FMenuBuilder& MenuBuilder, UEdGraph* Graph) const
{
	TSharedRef<SGraphEditorActionMenuAI> Menu =	
		SNew(SGraphEditorActionMenuAI)
		.GraphObj( Graph )
		.GraphNode((USelectionQueryGraphNode_Option*)this)
		.AutoExpandActionMenu(true);

	MenuBuilder.AddWidget(Menu,FText(),true);
}

void USelectionQueryGraphNode_Option::CalculateWeights()
{
	float MaxWeight = -1.0f;
	for (int32 Idx = 0; Idx < SubNodes.Num(); Idx++)
	{
		USelectionQueryGraphNode_Test* TestNode = Cast<USelectionQueryGraphNode_Test>(SubNodes[Idx]);
		if (TestNode == nullptr || !TestNode->bTestEnabled)
		{
			continue;
		}

		USelQueryTest* TestInstance = Cast<USelQueryTest>(TestNode->NodeInstance);
		if (TestInstance && !TestInstance->ScoringFactor.IsDynamic())
		{
			MaxWeight = FMath::Max(MaxWeight, FMath::Abs(TestInstance->ScoringFactor.DefaultValue));
		}
	}

	if (MaxWeight <= 0.0f)
	{
		MaxWeight = 1.0f;
	}

	for (int32 Idx = 0; Idx < SubNodes.Num(); Idx++)
	{
		USelectionQueryGraphNode_Test* TestNode = Cast<USelectionQueryGraphNode_Test>(SubNodes[Idx]);
		if (TestNode == NULL)
		{
			continue;
		}
		
		USelQueryTest* TestInstance = Cast<USelQueryTest>(TestNode->NodeInstance);
		const bool bHasDynamic = TestInstance && TestNode->bTestEnabled && TestInstance->ScoringFactor.IsDynamic();
		const float NewWeight = (TestInstance && TestNode->bTestEnabled) ?
			(TestInstance->ScoringFactor.IsDynamic() ? 1.0f : FMath::Clamp(FMath::Abs(TestInstance->ScoringFactor.DefaultValue) / MaxWeight, 0.0f, 1.0f)) :
			-1.0f;

		TestNode->SetDisplayedWeight(NewWeight, bHasDynamic);
	}
}

void USelectionQueryGraphNode_Option::UpdateNodeData()
{
	USelQueryOption* OptionInstance = Cast<USelQueryOption>(NodeInstance);
	USelQueryGenerator_Composite* CompositeGenerator = OptionInstance ? Cast<USelQueryGenerator_Composite>(OptionInstance->Generator) : nullptr;
	if (CompositeGenerator)
	{
		CompositeGenerator->VerifyItemTypes();

		ErrorMessage = CompositeGenerator->bHasMatchingItemType ? TEXT("") : LOCTEXT("NestedGeneratorMismatch", "Nested generators must work on exactly the same item types!").ToString();
	}
}


#undef LOCTEXT_NAMESPACE
