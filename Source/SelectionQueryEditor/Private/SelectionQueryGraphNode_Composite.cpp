// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "ScopedTransaction.h"
#include "SGraphEditorActionMenuAI.h"
#include "AI/SelectionQuery/SelQueryTest.h"
#include "AI/SelectionQuery/SelQueryOption.h"
#include "AI/SelectionQuery/Generators/SelectionQueryGenerator_Composite.h"
#include "SelectionQueryGraphNode_Composite.h"

#define LOCTEXT_NAMESPACE "SelectionQueryEditor"

USelectionQueryGraphNode_Composite::USelectionQueryGraphNode_Composite(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USelectionQueryGraphNode_Composite::AllocateDefaultPins()
{
	CreatePin(EGPD_Input,  TEXT("Transition"), TEXT(""), NULL, false, false, TEXT("In"));
	CreatePin(EGPD_Output, TEXT("Transition"), TEXT(""), NULL, false, false, TEXT("Out"));
}

void USelectionQueryGraphNode_Composite::PostPlacedNewNode()
{
	UClass* NodeClass = ClassData.GetClass(true);
	if (NodeClass)
	{
		UEdGraph* MyGraph = GetGraph();
		UObject* GraphOwner = MyGraph ? MyGraph->GetOuter() : nullptr;
		if (GraphOwner)
		{
			USQComposite* NewComposite = NewObject<USQComposite>(GraphOwner, NodeClass);
			NewComposite->UpdateNodeVersion();

			NewComposite->SetFlags(RF_Transactional);

			NodeInstance = NewComposite;
			InitializeInstance();
		}
	}
}

void USelectionQueryGraphNode_Composite::ResetNodeOwner()
{
	Super::ResetNodeOwner();

	USQComposite* OptionInstance = Cast<USQComposite>(NodeInstance);
	if (OptionInstance)
	{
		UObject* GraphOwner = GetGraph() ? GetGraph()->GetOuter() : nullptr;
		OptionInstance->Rename(NULL, GraphOwner, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

void USelectionQueryGraphNode_Composite::PrepareForCopying()
{
	Super::PrepareForCopying();

	USQComposite* OptionInstance = Cast<USQComposite>(NodeInstance);
	if (OptionInstance)
	{
		// Temporarily take ownership of the node instance, so that it is not deleted when cutting
		OptionInstance->Rename(NULL, this, REN_DontCreateRedirectors | REN_DoNotDirty );
	}
}

void USelectionQueryGraphNode_Composite::UpdateNodeClassData()
{
	USQComposite* CompositeInstance = Cast<USQComposite>(NodeInstance);
	if (CompositeInstance)
	{
		UpdateNodeClassDataFrom(CompositeInstance->GetClass(), ClassData);
		ErrorMessage = ClassData.GetDeprecatedMessage();
	}
}

FText USelectionQueryGraphNode_Composite::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	USQComposite* CompositeInstance = Cast<USQComposite>(NodeInstance);
	return CompositeInstance ? CompositeInstance->GetDescriptionTitle() : FText::GetEmpty();
}

FText USelectionQueryGraphNode_Composite::GetDescription() const
{
	USQComposite* CompositeInstance = Cast<USQComposite>(NodeInstance);
	return CompositeInstance ? CompositeInstance->GetDescriptionDetails() : FText::GetEmpty();
}

void USelectionQueryGraphNode_Composite::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
}

#undef LOCTEXT_NAMESPACE
