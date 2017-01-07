// Copyright 2015-2017 Piperift. All Rights Reserved.
#include "SelectionQueryEditorPrivatePCH.h"
#include "SGraphNode_SQItem.h"
#include "GraphEditor.h"
#include "SInlineEditableTextBlock.h"
#include "AssetThumbnail.h"
#include "AssetRegistryModule.h"
#include "SelectionQueryEditor.h"
#include "Graph/SQGraphNode.h"
#include "SNumericEntryBox.h"
#include "SLevelOfDetailBranchNode.h"

#include "SGraphNodeSQ.h"

#define LOCTEXT_NAMESPACE "SGraphNode_SQItem"


/** Widget for overlaying an execution-order index onto a node */
class SMeshGraphNodeIndex : public SCompoundWidget
{
public:
	/** Delegate event fired when the hover state of this widget changes */
	DECLARE_DELEGATE_OneParam(FOnHoverStateChanged, bool /* bHovered */);

	/** Delegate used to receive the color of the node, depending on hover state and state of other siblings */
	DECLARE_DELEGATE_RetVal_OneParam(FSlateColor, FOnGetIndexColor, bool /* bHovered */);

	SLATE_BEGIN_ARGS(SMeshGraphNodeIndex){}
	SLATE_ATTRIBUTE(FText, Text)
		SLATE_EVENT(FOnHoverStateChanged, OnHoverStateChanged)
		SLATE_EVENT(FOnGetIndexColor, OnGetIndexColor)
		SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		OnHoverStateChangedEvent = InArgs._OnHoverStateChanged;
		OnGetIndexColorEvent = InArgs._OnGetIndexColor;

		const FSlateBrush* IndexBrush = FEditorStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Index"));

		ChildSlot
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				// Add a dummy box here to make sure the widget doesn't get smaller than the brush
				SNew(SBox)
				.WidthOverride(IndexBrush->ImageSize.X)
				.HeightOverride(IndexBrush->ImageSize.Y)
			]
			+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SBorder)
					.BorderImage(IndexBrush)
					.BorderBackgroundColor(this, &SMeshGraphNodeIndex::GetColor)
					.Padding(FMargin(4.0f, 0.0f, 4.0f, 1.0f))
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Text(InArgs._Text)
						.Font(FEditorStyle::GetFontStyle("BTEditor.Graph.BTNode.IndexText"))
					]
				]
		];
	}

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		OnHoverStateChangedEvent.ExecuteIfBound(true);
		SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
	}

	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override
	{
		OnHoverStateChangedEvent.ExecuteIfBound(false);
		SCompoundWidget::OnMouseLeave(MouseEvent);
	}

	/** Get the color we use to display the rounded border */
	FSlateColor GetColor() const
	{
		if (OnGetIndexColorEvent.IsBound())
		{
			return OnGetIndexColorEvent.Execute(IsHovered());
		}

		return FSlateColor::UseForeground();
	}

private:
	/** Delegate event fired when the hover state of this widget changes */
	FOnHoverStateChanged OnHoverStateChangedEvent;

	/** Delegate used to receive the color of the node, depending on hover state and state of other siblings */
	FOnGetIndexColor OnGetIndexColorEvent;
};


void SGraphNode_SQItem::Construct(const FArguments& InArgs, USQGraphNode_Item* InNode)
{
	ThumbnailSize = FIntPoint(96, 96);
	GraphNode = InNode;
	EdActorNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	BuildThumbnailWidget();
	UpdateGraphNode();
}

static const FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);
static const FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
static const FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);

FSlateColor SGraphNode_SQItem::GetBorderBackgroundColor() const
{
	return InactiveStateColor;
}

const FSlateBrush* SGraphNode_SQItem::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}

void SGraphNode_SQItem::OnPropertyChanged(USQGraphNode* Sender, const FName& PropertyName)
{
	if (USQGraphNode_Item* ItemNode = Cast<USQGraphNode_Item>(Sender)) {
		SetThumbnail(ItemNode->GetThumbnailAssetObject());
		GraphNode->GetGraph()->NotifyGraphChanged();
	}
}

void SGraphNode_SQItem::SetThumbnail(UObject* AssetObject)
{
	AssetThumbnail->SetAsset(FAssetData(AssetObject));
}

void SGraphNode_SQItem::BuildThumbnailWidget()
{
	FObjectOrAssetData Value;
	GetValue(Value);
	AssetThumbnail = MakeShareable(new FAssetThumbnail(Value.AssetData, ThumbnailSize.X, ThumbnailSize.Y, FSQEditorThumbnailPool::Get()));
}

bool SGraphNode_SQItem::GetValue(FObjectOrAssetData& OutValue) const
{
	// Potentially accessing the value while garbage collecting or saving the package could trigger a crash.
	// so we fail to get the value when that is occurring.
	if (GIsSavingPackage || IsGarbageCollecting())
	{
		return false;
	}

	//UObject* Object = EdActorNode->GetNodeAssetObject();
	UObject* Object = EdActorNode->GetThumbnailAssetObject();
	OutValue = FObjectOrAssetData(Object);

	bool foundObject = (Object == NULL);
	return foundObject;
}

void SGraphNode_SQItem::UpdateGraphNode()
{
    InputPins.Empty();
    OutputPins.Empty();

    // Reset variables that are going to be exposed, in case we are refreshing an already setup node.
    RightNodeBox.Reset();
    LeftNodeBox.Reset();
    OutputPinBox.Reset();

    const FSlateBrush* NodeTypeIcon = GetNameIcon();

    FLinearColor BackgroundColor(0.1f, 0.1f, 0.1f);

    TSharedPtr<SErrorText> ErrorText;
    TSharedPtr<STextBlock> DescriptionText;
    TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

    TWeakPtr<SNodeTitle> WeakNodeTitle = NodeTitle;
    auto GetNodeTitlePlaceholderWidth = [WeakNodeTitle]() -> FOptionalSize
    {
        TSharedPtr<SNodeTitle> NodeTitlePin = WeakNodeTitle.Pin();
        const float DesiredWidth = (NodeTitlePin.IsValid()) ? NodeTitlePin->GetTitleSize().X : 0.0f;
        return FMath::Max(75.0f, DesiredWidth);
    };
    auto GetNodeTitlePlaceholderHeight = [WeakNodeTitle]() -> FOptionalSize
    {
        TSharedPtr<SNodeTitle> NodeTitlePin = WeakNodeTitle.Pin();
        const float DesiredHeight = (NodeTitlePin.IsValid()) ? NodeTitlePin->GetTitleSize().Y : 0.0f;
        return FMath::Max(22.0f, DesiredHeight);
    };

    const FMargin NodePadding = FMargin(8.0f);

    IndexOverlay = SNew(SMeshGraphNodeIndex)
        .ToolTipText(this, &SGraphNode_SQItem::GetIndexTooltipText)
        .Visibility(this, &SGraphNode_SQItem::GetIndexVisibility)
        .Text(this, &SGraphNode_SQItem::GetIndexText)
        .OnHoverStateChanged(this, &SGraphNode_SQItem::OnIndexHoverStateChanged)
        .OnGetIndexColor(this, &SGraphNode_SQItem::GetIndexColor);

    this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
    this->GetOrAddSlot(ENodeZone::Center)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Center)
        [
            SNew(SBorder)
            .BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
            .Padding(0.0f)
            .BorderBackgroundColor(this, &SGraphNode_SQItem::GetBorderBackgroundColor)
            [
                SNew(SOverlay)

                // Pins and node details
                + SOverlay::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                [
                    SNew(SVerticalBox)

                    // INPUT PIN AREA
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SBox)
                        .MinDesiredHeight(NodePadding.Top)
                        [
                            SAssignNew(LeftNodeBox, SVerticalBox)
                        ]
                    ]

                    // STATE NAME AREA
                    + SVerticalBox::Slot()
                    .Padding(FMargin(NodePadding.Left, 0.0f, NodePadding.Right, 0.0f))
                    [
                        SAssignNew(NodeBody, SBorder)
                        .BorderImage(FEditorStyle::GetBrush("BTEditor.Graph.BTNode.Body"))
                        .BorderBackgroundColor(BackgroundColor)
                        .HAlign(HAlign_Fill)
                        .VAlign(VAlign_Center)
                        .Visibility(EVisibility::SelfHitTestInvisible)
                        [
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            [
                                SNew(SLevelOfDetailBranchNode)
                                .UseLowDetailSlot(this, &SGraphNode_SQItem::UseLowDetailNodeTitles)
                                .LowDetail()
                                [
                                    SNew(SBox)
                                    .WidthOverride_Lambda(GetNodeTitlePlaceholderWidth)
                                    .HeightOverride_Lambda(GetNodeTitlePlaceholderHeight)
                                ]
                                .HighDetail()
                                [
                                    SNew(SHorizontalBox)
                                    + SHorizontalBox::Slot()
                                    .AutoWidth()
                                    .VAlign(VAlign_Center)
                                    [
                                        SNew(SImage)
                                        .Image(this, &SGraphNode_SQItem::GetNameIcon)
                                    ]

                                    + SHorizontalBox::Slot()
                                    .Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
                                    [
                                        SNew(SVerticalBox)
                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        [
                                            SAssignNew(InlineEditableText, SInlineEditableTextBlock)
                                            .Style(FEditorStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
                                            .Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
                                            .OnVerifyTextChanged(this, &SGraphNode_SQItem::OnVerifyNameTextChanged)
                                            .OnTextCommitted(this, &SGraphNode_SQItem::OnNameTextCommited)
                                            .IsReadOnly(this, &SGraphNode_SQItem::IsNameReadOnly)
                                            .IsSelected(this, &SGraphNode_SQItem::IsSelectedExclusively)
                                        ]

                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        [
                                            NodeTitle.ToSharedRef()
                                        ]
                                    ]
                                ]
                            ]

                            // THUMBNAIL AREA
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding(7.0f)
                            [
                                SNew(SBorder)
                                .BorderImage(FEditorStyle::GetBrush("BTEditor.Graph.BTNode.Body"))
                                .BorderBackgroundColor(BackgroundColor)
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                [
                                    SNew(SVerticalBox)
                                    // Thumbnail Slot
                                    + SVerticalBox::Slot()
                                    .FillHeight(1.0f)
                                    [
                                        SNew(SBox)
                                        .WidthOverride(ThumbnailSize.X)
                                        .HeightOverride(ThumbnailSize.Y)
                                        [
                                            AssetThumbnail->MakeThumbnailWidget()
                                        ]
                                    ]
                                ]
                            ]
                        ]
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        // DESCRIPTION MESSAGE
                        SAssignNew(DescriptionText, STextBlock)
                        .Visibility(this, &SGraphNode_SQItem::GetDescriptionVisibility)
                        .Text(this, &SGraphNode_SQItem::GetDescription)
                    ]
				]

				/*// OUTPUT PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Bottom)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.Padding(20.0f, 0.0f)
					.FillHeight(1.0f)
					[
						SAssignNew(OutputPinBox, SHorizontalBox)
					]
				]*/

                
				//+ SOverlay::Slot()
				//.HAlign(HAlign_Center)
				//.VAlign(VAlign_Center)
			]
		];

	CreatePinWidgets();
}

static USQGraphNode* GetParentNode(UEdGraphNode* GraphNode)
{
	USQGraphNode* StateNode = CastChecked<USQGraphNode>(GraphNode);
	UEdGraphPin* MyInputPin = StateNode->GetInputPin();
	UEdGraphPin* MyParentOutputPin = nullptr;
	if (MyInputPin != nullptr && MyInputPin->LinkedTo.Num() > 0)
	{
		MyParentOutputPin = MyInputPin->LinkedTo[0];
		if (MyParentOutputPin != nullptr)
		{
			if (MyParentOutputPin->GetOwningNode() != nullptr)
			{
				return CastChecked<USQGraphNode>(MyParentOutputPin->GetOwningNode());
			}
		}
	}

	return nullptr;
}

void SGraphNode_SQItem::OnIndexHoverStateChanged(bool bHovered)
{
	USQGraphNode* ParentNode = GetParentNode(GraphNode);
	if (ParentNode != nullptr)
	{
		//ParentNode->bHighlightChildNodeIndices = bHovered;
	}
}

FSlateColor SGraphNode_SQItem::GetIndexColor(bool bHovered) const
{
	USQGraphNode* ParentNode = GetParentNode(GraphNode);
	const bool bHighlightHover = bHovered/* || (ParentNode && ParentNode->bHighlightChildNodeIndices)*/;

	static const FName HoveredColor("BTEditor.Graph.BTNode.Index.HoveredColor");
	static const FName DefaultColor("BTEditor.Graph.BTNode.Index.Color");

	return bHovered ? FEditorStyle::Get().GetSlateColor(HoveredColor) : FEditorStyle::Get().GetSlateColor(DefaultColor);
}

EVisibility SGraphNode_SQItem::GetIndexVisibility() const
{
	return EVisibility::Visible;
}

FText SGraphNode_SQItem::GetIndexText() const
{
    USQGraphNode_Item* ActorNode = CastChecked<USQGraphNode_Item>(GraphNode);
	//return FText::AsNumber(ActorNode->ExecutionOrder);
    return FText::AsNumber(0);
}

FText SGraphNode_SQItem::GetIndexTooltipText() const
{
	if (GEditor->bIsSimulatingInEditor || GEditor->PlayWorld != NULL)
	{
		return LOCTEXT("ExecutionIndexTooltip", "Execution index: this shows the order in which nodes are executed.");
	}
	else
	{
		return LOCTEXT("ChildIndexTooltip", "Child index: this shows the order in which child nodes are executed.");
	}
}

void SGraphNode_SQItem::CreatePinWidgets()
{
    USQGraphNode_Item* Node = CastChecked<USQGraphNode_Item>(GraphNode);

    for (int32 PinIdx = 0; PinIdx < Node->Pins.Num(); PinIdx++)
    {
        UEdGraphPin* MyPin = Node->Pins[PinIdx];
        if (!MyPin->bHidden)
        {
            TSharedPtr<SGraphPin> NewPin = SNew(SSelectionQueryPin, MyPin);
            AddPin(NewPin.ToSharedRef());
        }
    }
}

FText SGraphNode_SQItem::GetDescription() const
{
    USQGraphNode* MyNode = CastChecked<USQGraphNode>(GraphNode);
    return MyNode ? MyNode->GetDescription() : FText::GetEmpty();
}

EVisibility SGraphNode_SQItem::GetDescriptionVisibility() const
{
    // LOD this out once things get too small
    TSharedPtr<SGraphPanel> MyOwnerPanel = GetOwnerPanel();
    return (!MyOwnerPanel.IsValid() || MyOwnerPanel->GetCurrentLOD() > EGraphRenderingLOD::LowDetail) ? EVisibility::Visible : EVisibility::Collapsed;
}

void SGraphNode_SQItem::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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
			.Padding(20.0f, 0.0f)
			[
				PinToAdd
			];
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
	{
		OutputPinBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(1.0f)
			[
				PinToAdd
			];
		OutputPins.Add(PinToAdd);
	}
}


TArray<FOverlayWidgetInfo> SGraphNode_SQItem::GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const
{
	TArray<FOverlayWidgetInfo> Widgets;
	check(IndexOverlay.IsValid());

	FVector2D Origin(0.0f, 0.0f);

	FOverlayWidgetInfo Overlay(IndexOverlay);
	Overlay.OverlayOffset = FVector2D(WidgetSize.X - (IndexOverlay->GetDesiredSize().X * 0.5f), Origin.Y);
	Widgets.Add(Overlay);


	return Widgets;
}


void SGraphNode_SQItem::RebuildExecutionOrder() {
	USQGraphNode* Parent = GetParentNode(GraphNode);
	if (Parent) {
		//Parent->UpdateChildExecutionOrder();
	}
}


void SGraphNode_SQItem::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter)
{
	RebuildExecutionOrder();
	return SGraphNode::MoveTo(NewPosition, NodeFilter);
}

#undef LOCTEXT_NAMESPACE