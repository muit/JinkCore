// Copyright 2015-2017 Piperift. All Rights Reserved.
#include "SelectionQueryEditorPrivatePCH.h"
#include "SGraphNode.h"
#include "SGraphNodeSQ.h"
#include "SGraphPin.h"
#include "Editor/GraphEditor/Private/SGraphPanel.h"
#include "ScopedTransaction.h"


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
    return FSlateColor(IsHovered() ? FLinearColor::Yellow : FLinearColor(0.01f, 0.01f, 0.01f));
}

void SGraphNodeSQ::Construct(const FArguments& InArgs, USQGraphNode* InNode)
{
    SetCursor(EMouseCursor::CardinalCross);

    GraphNode = InNode;
    UpdateGraphNode();

    bDragMarkerVisible = false;
}

FText SGraphNodeSQ::GetDescription() const
{
    USQGraphNode* MyNode = CastChecked<USQGraphNode>(GraphNode);
    return MyNode ? MyNode->GetDescription() : FText::GetEmpty();
}

EVisibility SGraphNodeSQ::GetDescriptionVisibility() const
{
    // LOD this out once things get too small
    TSharedPtr<SGraphPanel> MyOwnerPanel = GetOwnerPanel();
    return (!MyOwnerPanel.IsValid() || MyOwnerPanel->GetCurrentLOD() > EGraphRenderingLOD::LowDetail) ? EVisibility::Visible : EVisibility::Collapsed;
}

void SGraphNodeSQ::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
    PinToAdd->SetOwner(SharedThis(this));

    const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
    const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
    if (bAdvancedParameter)
    {
        PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
    }

    if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
    {
        LeftNodeBox->AddSlot()
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            .FillHeight(1.0f)
            [
                PinToAdd
            ];
        InputPins.Add(PinToAdd);
    }
    else // Direction == EEdGraphPinDirection::EGPD_Output
    {
        RightNodeBox->AddSlot()
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            .FillHeight(1.0f)
            [
                PinToAdd
            ];
        OutputPins.Add(PinToAdd);
    }
}

TSharedPtr<SToolTip> SGraphNodeSQ::GetComplexTooltip()
{
    return NULL;
}

FText SGraphNodeSQ::GetPreviewCornerText() const
{
    return FText::GetEmpty();
}

const FSlateBrush* SGraphNodeSQ::GetNameIcon() const
{
    return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}

#undef LOCTEXT_NAMESPACE
