// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.


#include "SelectionQueryEditorPrivatePCH.h"
#include "SGraphPreviewer.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "NodeFactory.h"
#include "SGraphNode.h"
#include "SGraphNode_SelectionQuery.h"
#include "SGraphPin.h"
#include "SGraphPanel.h"
#include "ScopedTransaction.h"
#include "SelectionQueryColors.h"
#include "SelectionQuery/SelQueryTest.h"
#include "SInlineEditableTextBlock.h"
#include "SLevelOfDetailBranchNode.h"

#define LOCTEXT_NAMESPACE "SelectionQueryEditor"

class SSelectionQueryPin : public SGraphPinAI
{
public:
	SLATE_BEGIN_ARGS(SSelectionQueryPin){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

	virtual FSlateColor GetPinColor() const override;
};

void SSelectionQueryPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPinAI::Construct(SGraphPinAI::FArguments(), InPin);
}

FSlateColor SSelectionQueryPin::GetPinColor() const
{
	return IsHovered() ? SelectionQueryColors::Pin::Hover :
		SelectionQueryColors::Pin::Default;
}

/////////////////////////////////////////////////////
// SGraphNode_SelectionQuery

void SGraphNode_SelectionQuery::Construct(const FArguments& InArgs, USelectionQueryGraphNode* InNode)
{
	SGraphNodeAI::Construct(SGraphNodeAI::FArguments(), InNode);
}

void SGraphNode_SelectionQuery::AddSubNode(TSharedPtr<SGraphNode> SubNodeWidget)
{
	SGraphNodeAI::AddSubNode(SubNodeWidget);

	TestBox->AddSlot().AutoHeight()
		[
			SubNodeWidget.ToSharedRef()
		];
}

FSlateColor SGraphNode_SelectionQuery::GetBorderBackgroundColor() const
{
	USelectionQueryGraphNode_Test* TestNode = Cast<USelectionQueryGraphNode_Test>(GraphNode);
	const bool bSelectedSubNode = TestNode && TestNode->ParentNode && GetOwnerPanel().IsValid() && GetOwnerPanel()->SelectionManager.SelectedNodes.Contains(GraphNode);

	return bSelectedSubNode ? SelectionQueryColors::NodeBorder::Selected :
		SelectionQueryColors::NodeBorder::Default;
}

FSlateColor SGraphNode_SelectionQuery::GetBackgroundColor() const
{
	const USelectionQueryGraphNode* MyNode = Cast<USelectionQueryGraphNode>(GraphNode);
	const UClass* MyClass = MyNode && MyNode->NodeInstance ? MyNode->NodeInstance->GetClass() : NULL;
	
	FLinearColor NodeColor = SelectionQueryColors::NodeBody::Default;
	if (MyClass)
	{
		if (MyClass->IsChildOf( USelQueryTest::StaticClass() ))
		{
			USelectionQueryGraphNode_Test* MyTestNode = Cast<USelectionQueryGraphNode_Test>(GraphNode);
			NodeColor = (MyTestNode && MyTestNode->bTestEnabled) ? SelectionQueryColors::NodeBody::Test : SelectionQueryColors::NodeBody::TestInactive;
		}
		else if (MyClass->IsChildOf( USelQueryOption::StaticClass() ))
		{
			NodeColor = SelectionQueryColors::NodeBody::Generator;
		}
	}

	if (MyNode->HasErrors())
	{
		NodeColor = SelectionQueryColors::NodeBody::Error;
	}

	return NodeColor;
}

void SGraphNode_SelectionQuery::UpdateGraphNode()
{
	if (TestBox.IsValid())
	{
		TestBox->ClearChildren();
	} 
	else
	{
		SAssignNew(TestBox,SVerticalBox);
	}

	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	SubNodes.Reset();

	const FMargin NodePadding = (Cast<USelectionQueryGraphNode_Test>(GraphNode) != NULL) 
		? FMargin(2.0f) 
		: FMargin(8.0f);
	float TestPadding = 0.0f;

	USelectionQueryGraphNode_Option* OptionNode = Cast<USelectionQueryGraphNode_Option>(GraphNode);
	if (OptionNode)
	{
		for (int32 Idx = 0; Idx < OptionNode->SubNodes.Num(); Idx++)
		{
			if (OptionNode->SubNodes[Idx])
			{
				TSharedPtr<SGraphNode> NewNode = FNodeFactory::CreateNodeWidget(OptionNode->SubNodes[Idx]);
				if (OwnerGraphPanelPtr.IsValid())
				{
					NewNode->SetOwner(OwnerGraphPanelPtr.Pin().ToSharedRef());
					OwnerGraphPanelPtr.Pin()->AttachGraphEvents(NewNode);
				}
				AddSubNode(NewNode);
				NewNode->UpdateGraphNode();
			}
		}

		if (SubNodes.Num() == 0)
		{
			TestBox->AddSlot().AutoHeight()
				[
					SNew(STextBlock).Text(LOCTEXT("NoTests","Right click to add tests"))
				];
		}

		TestPadding = 10.0f;
	}

	const FSlateBrush* NodeTypeIcon = GetNameIcon();

	FLinearColor TitleShadowColor(1.0f, 0.0f, 0.0f);
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

	this->ContentScale.Bind( this, &SGraphNode::GetContentScale );
	this->GetOrAddSlot( ENodeZone::Center )
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage( FEditorStyle::GetBrush( "Graph.StateNode.Body" ) )
			.Padding(0)
			.BorderBackgroundColor( this, &SGraphNode_SelectionQuery::GetBorderBackgroundColor )
			.OnMouseButtonDown(this, &SGraphNode_SelectionQuery::OnMouseDown)
			[
				SNew(SOverlay)

				// Pins and node details
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SVerticalBox)

					// INPUT PIN AREA
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.MinDesiredHeight(NodePadding.Top)
						[
							SAssignNew(LeftNodeBox, SVerticalBox)
						]
					]

					// STATE NAME AREA
					+SVerticalBox::Slot()
					.Padding(FMargin(NodePadding.Left, 0.0f, NodePadding.Right, 0.0f))
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.AutoWidth()
						[
							SNew(SCheckBox)
							.Visibility(this, &SGraphNode_SelectionQuery::GetTestToggleVisibility)
							.IsChecked(this, &SGraphNode_SelectionQuery::IsTestToggleChecked)
							.OnCheckStateChanged(this, &SGraphNode_SelectionQuery::OnTestToggleChanged)
							.Padding(FMargin(0, 0, 4.0f, 0))
						]
						+SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SVerticalBox)
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(SBorder)
								.BorderImage( FEditorStyle::GetBrush("Graph.StateNode.Body") )
								.BorderBackgroundColor( this, &SGraphNode_SelectionQuery::GetBackgroundColor )
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Center)
								.Visibility(EVisibility::SelfHitTestInvisible)
								[
									SNew(SVerticalBox)
									+SVerticalBox::Slot()
									.AutoHeight()
									.Padding(0,0,0,2)
									[
										SNew(SBox).HeightOverride(4)
										[
											// weight bar
											SNew(SProgressBar)
											.FillColorAndOpacity(this, &SGraphNode_SelectionQuery::GetWeightProgressBarColor)
											.Visibility(this, &SGraphNode_SelectionQuery::GetWeightMarkerVisibility)
											.Percent(this, &SGraphNode_SelectionQuery::GetWeightProgressBarPercent)
										]
									]
									+SVerticalBox::Slot()
									.AutoHeight()
									[
										SNew(SHorizontalBox)
										+SHorizontalBox::Slot()
										.AutoWidth()
										[
											// POPUP ERROR MESSAGE
											SAssignNew(ErrorText, SErrorText )
											.BackgroundColor( this, &SGraphNode_SelectionQuery::GetErrorColor )
											.ToolTipText( this, &SGraphNode_SelectionQuery::GetErrorMsgToolTip )
										]
										+SHorizontalBox::Slot()
										.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
										[
											SNew(SLevelOfDetailBranchNode)
											.UseLowDetailSlot(this, &SGraphNode_SelectionQuery::UseLowDetailNodeTitles)
											.LowDetail()
											[
												SNew(SBox)
												.WidthOverride_Lambda(GetNodeTitlePlaceholderWidth)
												.HeightOverride_Lambda(GetNodeTitlePlaceholderHeight)
											]
											.HighDetail()
											[
												SNew(SVerticalBox)
												+SVerticalBox::Slot()
												.AutoHeight()
												[
													SAssignNew(InlineEditableText, SInlineEditableTextBlock)
													.Style( FEditorStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText" )
													.Text( NodeTitle.Get(), &SNodeTitle::GetHeadTitle )
													.OnVerifyTextChanged(this, &SGraphNode_SelectionQuery::OnVerifyNameTextChanged)
													.OnTextCommitted(this, &SGraphNode_SelectionQuery::OnNameTextCommited)
													.IsReadOnly( this, &SGraphNode_SelectionQuery::IsNameReadOnly )
													.IsSelected(this, &SGraphNode_SelectionQuery::IsSelectedExclusively)
												]
												+SVerticalBox::Slot()
												.AutoHeight()
												[
													NodeTitle.ToSharedRef()
												]
											]
										]
									]
									+SVerticalBox::Slot()
									.AutoHeight()
									[
										// DESCRIPTION MESSAGE
										SAssignNew(DescriptionText, STextBlock )
										.Visibility(this, &SGraphNode_SelectionQuery::GetDescriptionVisibility)
										.Text(this, &SGraphNode_SelectionQuery::GetDescription)
									]
								]
							]
							+SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0, TestPadding, 0, 0)
							[
								TestBox.ToSharedRef()
							]
						]
					]

					// OUTPUT PIN AREA
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.MinDesiredHeight(NodePadding.Bottom)
						[
							SAssignNew(RightNodeBox, SVerticalBox)
						]
					]
				]

				// Drag marker overlay
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SNew(SBorder)
					.BorderBackgroundColor(SelectionQueryColors::Action::DragMarker)
					.ColorAndOpacity(SelectionQueryColors::Action::DragMarker)
					.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
					.Visibility(this, &SGraphNode_SelectionQuery::GetDragOverMarkerVisibility)
					[
						SNew(SBox)
						.HeightOverride(4)
					]
				]
			]
		];

	// Create comment bubble
	TSharedPtr<SCommentBubble> CommentBubble;
	const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;

	SAssignNew(CommentBubble, SCommentBubble)
		.GraphNode(GraphNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted( this, &SGraphNode::OnCommentTextCommitted )
		.ColorAndOpacity(CommentColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
		.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
		.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
		.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
		.VAlign(VAlign_Top)
		[
			CommentBubble.ToSharedRef()
		];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	CreatePinWidgets();
}

void SGraphNode_SelectionQuery::CreatePinWidgets()
{
	USelectionQueryGraphNode* StateNode = CastChecked<USelectionQueryGraphNode>(GraphNode);

	UEdGraphPin* CurPin = StateNode->GetOutputPin();
	if (CurPin && !CurPin->bHidden)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SSelectionQueryPin, CurPin);

		AddPin(NewPin.ToSharedRef());
	}

	CurPin = StateNode->GetInputPin();
	if (CurPin && !CurPin->bHidden)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SSelectionQueryPin, CurPin);

		AddPin(NewPin.ToSharedRef());
	}
}

EVisibility SGraphNode_SelectionQuery::GetWeightMarkerVisibility() const
{
	USelectionQueryGraphNode_Test* MyTestNode = Cast<USelectionQueryGraphNode_Test>(GraphNode);
	return MyTestNode ? EVisibility::Visible : EVisibility::Collapsed;
}

TOptional<float> SGraphNode_SelectionQuery::GetWeightProgressBarPercent() const
{
	USelectionQueryGraphNode_Test* MyTestNode = Cast<USelectionQueryGraphNode_Test>(GraphNode);
	return MyTestNode ? FMath::Max(0.0f, MyTestNode->TestWeightPct) : TOptional<float>();
}

FSlateColor SGraphNode_SelectionQuery::GetWeightProgressBarColor() const
{
	USelectionQueryGraphNode_Test* MyTestNode = Cast<USelectionQueryGraphNode_Test>(GraphNode);
	return (MyTestNode && MyTestNode->bHasNamedWeight) ? SelectionQueryColors::Action::WeightNamed : SelectionQueryColors::Action::Weight;
}

EVisibility SGraphNode_SelectionQuery::GetTestToggleVisibility() const
{
	USelectionQueryGraphNode_Test* MyTestNode = Cast<USelectionQueryGraphNode_Test>(GraphNode);
	return MyTestNode ? EVisibility::Visible : EVisibility::Collapsed;
}

ECheckBoxState SGraphNode_SelectionQuery::IsTestToggleChecked() const
{
	USelectionQueryGraphNode_Test* MyTestNode = Cast<USelectionQueryGraphNode_Test>(GraphNode);
	return MyTestNode && MyTestNode->bTestEnabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SGraphNode_SelectionQuery::OnTestToggleChanged(ECheckBoxState NewState)
{
	USelectionQueryGraphNode_Test* MyTestNode = Cast<USelectionQueryGraphNode_Test>(GraphNode);
	if (MyTestNode)
	{
		MyTestNode->bTestEnabled = (NewState == ECheckBoxState::Checked);
		
		USelectionQueryGraphNode_Option* MyParentNode = Cast<USelectionQueryGraphNode_Option>(MyTestNode->ParentNode);
		if (MyParentNode)
		{
			MyParentNode->CalculateWeights();
		}

		USelectionQueryGraph* MyGraph = Cast<USelectionQueryGraph>(MyTestNode->GetGraph());
		if (MyGraph)
		{
			MyGraph->UpdateAsset();
		}
	}
}

#undef LOCTEXT_NAMESPACE
