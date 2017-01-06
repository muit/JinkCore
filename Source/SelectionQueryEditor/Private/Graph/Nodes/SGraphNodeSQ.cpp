// Copyright 2015-2017 Piperift. All Rights Reserved.
#include "SelectionQueryEditorPrivatePCH.h"
#include "SGraphPin.h"


#define LOCTEXT_NAMESPACE "SQGraph"

void SSelectionQueryPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SSelectionQueryPin::GetPinBorder)
		.BorderBackgroundColor(this, &SSelectionQueryPin::GetPinColor)
		.OnMouseButtonDown(this, &SSelectionQueryPin::OnPinMouseDown)
		.Cursor(this, &SSelectionQueryPin::GetPinCursor)
		.Padding(FMargin(10.0f))
		);
}

TSharedRef<SWidget>	SSelectionQueryPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SSelectionQueryPin::GetPinBorder() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Body"));
}

FSlateColor SSelectionQueryPin::GetPinColor() const
{
	return FSlateColor(IsHovered() ? FLinearColor::Yellow : FLinearColor::Black);
}

#undef LOCTEXT_NAMESPACE