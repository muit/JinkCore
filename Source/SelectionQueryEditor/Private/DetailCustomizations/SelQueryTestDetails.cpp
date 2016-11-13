/ Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "SelQueryTestDetails.h"
#include "SelectionQuery/SelectionQueryTypes.h"
#include "SelectionQuery/SelQueryTest.h"

#define LOCTEXT_NAMESPACE "SelQueryTestDetails"

TSharedRef<IDetailCustomization> FSelQueryTestDetails::MakeInstance()
{
	return MakeShareable( new FSelQueryTestDetails );
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FSelQueryTestDetails::CustomizeDetails( IDetailLayoutBuilder& DetailLayout )
{
	AllowWriting = false;

	TArray<TWeakObjectPtr<UObject> > EditedObjects;
	DetailLayout.GetObjectsBeingCustomized(EditedObjects);
	for (int32 i = 0; i < EditedObjects.Num(); i++)
	{
		const USelQueryTest* EditedTest = Cast<const USelQueryTest>(EditedObjects[i].Get());
		if (EditedTest)
		{
			MyTest = EditedTest;
			break;
		}
	}

	// Initialize all handles
	FilterTypeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, FilterType));
	ScoreEquationHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, ScoringEquation));
	TestPurposeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, TestPurpose));
	ScoreHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, ScoringFactor));

	ClampMinTypeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, ClampMinType));
	ClampMaxTypeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, ClampMaxType));

	ScoreClampMinHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, ScoreClampMin));
	FloatValueMinHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, FloatValueMin));

	ScoreClampMaxHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, ScoreClampMax));
	FloatValueMaxHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, FloatValueMax));

	// Build combo box data values
	BuildFilterTestValues();
	BuildScoreEquationValues();
	BuildScoreClampingTypeValues(true, ClampMinTypeValues);
	BuildScoreClampingTypeValues(false, ClampMaxTypeValues);
	
	IDetailCategoryBuilder& TestCategory = DetailLayout.EditCategory("Test");
	IDetailPropertyRow& TestPurposeRow = TestCategory.AddProperty(TestPurposeHandle);

	IDetailCategoryBuilder& FilterCategory = DetailLayout.EditCategory("Filter");

	IDetailPropertyRow& FilterTypeRow = FilterCategory.AddProperty(FilterTypeHandle);
	FilterTypeRow.CustomWidget()
		.NameContent()
		[
			FilterTypeHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SComboButton)
			.OnGetMenuContent(this, &FSelQueryTestDetails::OnGetFilterTestContent)
			.ContentPadding(FMargin( 2.0f, 2.0f ))
			.ButtonContent()
			[
				SNew(STextBlock) 
				.Text(this, &FSelQueryTestDetails::GetCurrentFilterTestDesc)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];
  	FilterTypeRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetFloatFilterVisibility)));

	// filters
	IDetailPropertyRow& FloatValueMinRow = FilterCategory.AddProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, FloatValueMin)));
	FloatValueMinRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetVisibilityOfFloatValueMin)));

	IDetailPropertyRow& FloatValueMaxRow = FilterCategory.AddProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, FloatValueMax)));
	FloatValueMaxRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetVisibilityOfFloatValueMax)));

	IDetailPropertyRow& BoolValueRow = FilterCategory.AddProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, BoolValue)));
	BoolValueRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetBoolValueVisibility)));

	// required when it's created for "scoring only" tests
	IDetailGroup& HackToEnsureFilterCategoryIsVisible = FilterCategory.AddGroup("HackForVisibility", FText::GetEmpty());
	HackToEnsureFilterCategoryIsVisible.HeaderRow().Visibility(EVisibility::Hidden);

	// Scoring
	IDetailCategoryBuilder& ScoreCategory = DetailLayout.EditCategory("Score");

	//----------------------------
	// BEGIN Scoring: Clamping
	IDetailGroup& ClampingGroup = ScoreCategory.AddGroup("Clamping", LOCTEXT("ClampingLabel", "Clamping"));
	ClampingGroup.HeaderRow()
	[
		SNew(STextBlock)
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetClampingVisibility)))
		.Text(FText::FromString("Clamping"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	];
	
	// Drop-downs for setting type of lower and upper bound normalization
	IDetailPropertyRow& ClampMinTypeRow = ClampingGroup.AddPropertyRow(ClampMinTypeHandle.ToSharedRef());
	ClampMinTypeRow.CustomWidget()
		.NameContent()
		[
			ClampMinTypeHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SComboButton)
			.OnGetMenuContent(this, &FSelQueryTestDetails::OnGetClampMinTypeContent)
			.ContentPadding(FMargin( 2.0f, 2.0f ))
			.ButtonContent()
			[
				SNew(STextBlock) 
				.Text(this, &FSelQueryTestDetails::GetClampMinTypeDesc)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];
	ClampMinTypeRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetFloatScoreVisibility)));

	// Lower Bound for normalization of score if specified independently of filtering.
	IDetailPropertyRow& ScoreClampMinRow = ClampingGroup.AddPropertyRow(ScoreClampMinHandle.ToSharedRef());
	ScoreClampMinRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetVisibilityOfScoreClampMinimum)));

	// Lower Bound for scoring when tied to filter minimum.
	if (FloatValueMinHandle->IsValidHandle())
	{
		IDetailPropertyRow& FloatValueMinForClampingRow = ClampingGroup.AddPropertyRow(FloatValueMinHandle.ToSharedRef());
		FloatValueMinForClampingRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetVisibilityOfValueMinForScoreClamping)));
		FloatValueMinForClampingRow.ToolTip(LOCTEXT("FloatFilterMinForClampingRowToolTip", "See Filter Thresholds under the Filter tab.  Values lower than this (before clamping) cause the item to be thrown out as invalid.  Values are normalized with this value as the minimum, so items with this value will have a normalized score of 0."));
		FloatValueMinForClampingRow.EditCondition(TAttribute<bool>(this, &FSelQueryTestDetails::AllowWritingToFiltersFromScore), NULL);
	}

	if (ClampMaxTypeHandle->IsValidHandle())
	{
		IDetailPropertyRow& ClampMaxTypeRow = ClampingGroup.AddPropertyRow(ClampMaxTypeHandle.ToSharedRef());
		ClampMaxTypeRow.CustomWidget()
			.NameContent()
			[
				ClampMaxTypeHandle->CreatePropertyNameWidget()
			]
			.ValueContent()
			[
				SNew(SComboButton)
				.OnGetMenuContent(this, &FSelQueryTestDetails::OnGetClampMaxTypeContent)
				.ContentPadding(FMargin( 2.0f, 2.0f ))
				.ButtonContent()
				[
					SNew(STextBlock) 
					.Text(this, &FSelQueryTestDetails::GetClampMaxTypeDesc)
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
			];
		ClampMaxTypeRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetFloatScoreVisibility)));
	}

	// Upper Bound for normalization of score if specified independently of filtering.
	if (ScoreClampMaxHandle->IsValidHandle())
	{
		IDetailPropertyRow& ScoreClampMaxRow = ClampingGroup.AddPropertyRow(ScoreClampMaxHandle.ToSharedRef());
		ScoreClampMaxRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetVisibilityOfScoreClampMaximum)));
	}
	
	if (FloatValueMaxHandle->IsValidHandle())
	{
		// Upper Bound for scoring when tied to filter maximum.
		IDetailPropertyRow& FloatValueMaxForClampingRow = ClampingGroup.AddPropertyRow(FloatValueMaxHandle.ToSharedRef());
		FloatValueMaxForClampingRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetVisibilityOfValueMaxForScoreClamping)));
		FloatValueMaxForClampingRow.ToolTip(LOCTEXT("FloatFilterMaxForClampingRowToolTip", "See Filter Thresholds under the Filter tab.  Values higher than this (before normalization) cause the item to be thrown out as invalid.  Values are normalized with this value as the maximum, so items with this value will have a normalized score of 1."));
		FloatValueMaxForClampingRow.EditCondition(TAttribute<bool>(this, &FSelQueryTestDetails::AllowWritingToFiltersFromScore), NULL);
	}

	// END Scoring: Clamping, continue Scoring
	//----------------------------

	IDetailPropertyRow& BoolScoreTestRow = ScoreCategory.AddProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, BoolValue)));
	BoolScoreTestRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetBoolValueVisibilityForScoring)));
	BoolScoreTestRow.DisplayName(LOCTEXT("BoolMatchLabel", "Bool Match"));
	BoolScoreTestRow.ToolTip(LOCTEXT("BoolMatchToolTip", "Boolean value to match in order to grant score of 'ScoringFactor'.  Not matching this value will not change score."));

// 	IDetailPropertyRow& ScoreMirrorNormalizedScoreRow = ScoreCategory.AddProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, bMirrorNormalizedScore)));
// 	ScoreMirrorNormalizedScoreRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetFloatScoreVisibility)));

	IDetailPropertyRow& ScoreEquationTypeRow = ScoreCategory.AddProperty(ScoreEquationHandle);
	ScoreEquationTypeRow.CustomWidget()
		.NameContent()
		.VAlign(VAlign_Top)
		[
			ScoreEquationHandle->CreatePropertyNameWidget()
		]
		.ValueContent().MaxDesiredWidth(600)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			[
				SNew(SComboButton)
				.OnGetMenuContent(this, &FSelQueryTestDetails::OnGetEquationValuesContent)
				.ContentPadding(FMargin( 2.0f, 2.0f ))
				.ButtonContent()
				[
					SNew(STextBlock)
					.Text(this, &FSelQueryTestDetails::GetEquationValuesDesc)
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
			]
			+SVerticalBox::Slot()
			.Padding(0, 2, 0, 0)
			.AutoHeight()
			[
				SNew(STextBlock)
				.IsEnabled(false)
				.Text(this, &FSelQueryTestDetails::GetScoreEquationInfo)
				.Font( IDetailLayoutBuilder::GetDetailFont() )
			]
		];
	ScoreEquationTypeRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetFloatScoreVisibility)));

	IDetailPropertyRow& ScoreFactorRow = ScoreCategory.AddProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(USelQueryTest, ScoringFactor)));
	ScoreFactorRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelQueryTestDetails::GetScoreVisibility)));

	// scoring & filter function preview
	IDetailCategoryBuilder& PreviewCategory = DetailLayout.EditCategory("Preview");
	PreviewCategory.AddCustomRow(LOCTEXT("Preview", "Preview")).WholeRowWidget
	[
		SAssignNew(PreviewWidget, STestFunctionWidget)
	];

	PreviewWidget->DrawTestOb = Cast<USelQueryTest>(MyTest.Get());
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FSelQueryTestDetails::BuildFilterTestValues()
{
	UEnum* TestConditionEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnvTestFilterType"));
	check(TestConditionEnum);

	FilterTestValues.Reset();

	const USelQueryTest* EditedTest = Cast<const USelQueryTest>(MyTest.Get());
	if (EditedTest)
	{
		if (EditedTest->GetWorkOnFloatValues())
		{
			FilterTestValues.Add(FTextIntPair(TestConditionEnum->GetEnumText(EEnvTestFilterType::Minimum), EEnvTestFilterType::Minimum));
			FilterTestValues.Add(FTextIntPair(TestConditionEnum->GetEnumText(EEnvTestFilterType::Maximum), EEnvTestFilterType::Maximum));
			FilterTestValues.Add(FTextIntPair(TestConditionEnum->GetEnumText(EEnvTestFilterType::Range), EEnvTestFilterType::Range));
		}
		else
		{
			FilterTestValues.Add(FTextIntPair(TestConditionEnum->GetEnumText(EEnvTestFilterType::Match), EEnvTestFilterType::Match));
		}
	}
}

void FSelQueryTestDetails::BuildScoreEquationValues()
{
	UEnum* TestScoreEquationEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnvTestScoreEquation"));
	check(TestScoreEquationEnum);

	ScoreEquationValues.Reset();

	// Const scoring is always valid.  But other equations are only valid if the score values can be other than boolean values.
	ScoreEquationValues.Add(FTextIntPair(TestScoreEquationEnum->GetEnumText(EEnvTestScoreEquation::Constant), EEnvTestScoreEquation::Constant));

	const USelQueryTest* EditedTest = Cast<const USelQueryTest>(MyTest.Get());
	if (EditedTest)
	{
		if (EditedTest->GetWorkOnFloatValues())
		{
			ScoreEquationValues.Add(FTextIntPair(TestScoreEquationEnum->GetEnumText(EEnvTestScoreEquation::Linear), EEnvTestScoreEquation::Linear));
			ScoreEquationValues.Add(FTextIntPair(TestScoreEquationEnum->GetEnumText(EEnvTestScoreEquation::Square), EEnvTestScoreEquation::Square));
			ScoreEquationValues.Add(FTextIntPair(TestScoreEquationEnum->GetEnumText(EEnvTestScoreEquation::InverseLinear), EEnvTestScoreEquation::InverseLinear));
			ScoreEquationValues.Add(FTextIntPair(TestScoreEquationEnum->GetEnumText(EEnvTestScoreEquation::SquareRoot), EEnvTestScoreEquation::SquareRoot));
		}
	}
}

void FSelQueryTestDetails::OnFilterTestChange(int32 Index)
{
	uint8 EnumValue = Index;
	FilterTypeHandle->SetValue(EnumValue);
}

void FSelQueryTestDetails::OnScoreEquationChange(int32 Index)
{
	uint8 EnumValue = Index;
	ScoreEquationHandle->SetValue(EnumValue);
}

void FSelQueryTestDetails::BuildScoreClampingTypeValues(bool bBuildMinValues, TArray<FTextIntPair>& ClampTypeValues) const
{
	UEnum* ScoringNormalizationEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESelQueryTestClamping"));
	check(ScoringNormalizationEnum);

	ClampTypeValues.Reset();
	ClampTypeValues.Add(FTextIntPair(ScoringNormalizationEnum->GetEnumText(ESelQueryTestClamping::None), ESelQueryTestClamping::None));
	ClampTypeValues.Add(FTextIntPair(ScoringNormalizationEnum->GetEnumText(ESelQueryTestClamping::SpecifiedValue), ESelQueryTestClamping::SpecifiedValue));

	if (IsFiltering())
	{
		bool bSupportFilterThreshold = false;
		if (bBuildMinValues)
		{
			if (UsesFilterMin())
			{
				bSupportFilterThreshold = true;
			}
		}
		else if (UsesFilterMax())
		{
			bSupportFilterThreshold = true;
		}

		if (bSupportFilterThreshold)
		{
			ClampTypeValues.Add(FTextIntPair(ScoringNormalizationEnum->GetEnumText(ESelQueryTestClamping::FilterThreshold), ESelQueryTestClamping::FilterThreshold));
		}
	}
}

void FSelQueryTestDetails::OnClampMinTestChange(int32 Index)
{
	check(ClampMinTypeHandle.IsValid());
	uint8 EnumValue = Index;
	ClampMinTypeHandle->SetValue(EnumValue);
}

void FSelQueryTestDetails::OnClampMaxTestChange(int32 Index)
{
	check(ClampMaxTypeHandle.IsValid());
	uint8 EnumValue = Index;
	ClampMaxTypeHandle->SetValue(EnumValue);
}

TSharedRef<SWidget> FSelQueryTestDetails::OnGetClampMinTypeContent()
{
 	BuildScoreClampingTypeValues(true, ClampMinTypeValues);
	FMenuBuilder MenuBuilder(true, NULL);

 	for (int32 i = 0; i < ClampMinTypeValues.Num(); i++)
 	{
 		FUIAction ItemAction(FExecuteAction::CreateSP(this, &FSelQueryTestDetails::OnClampMinTestChange, ClampMinTypeValues[i].Int));
 		MenuBuilder.AddMenuEntry(ClampMinTypeValues[i].Text, TAttribute<FText>(), FSlateIcon(), ItemAction);
 	}

	return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget> FSelQueryTestDetails::OnGetClampMaxTypeContent()
{
 	BuildScoreClampingTypeValues(false, ClampMaxTypeValues);
	FMenuBuilder MenuBuilder(true, NULL);

 	for (int32 i = 0; i < ClampMaxTypeValues.Num(); i++)
 	{
 		FUIAction ItemAction(FExecuteAction::CreateSP(this, &FSelQueryTestDetails::OnClampMaxTestChange, ClampMaxTypeValues[i].Int));
 		MenuBuilder.AddMenuEntry(ClampMaxTypeValues[i].Text, TAttribute<FText>(), FSlateIcon(), ItemAction);
 	}

	return MenuBuilder.MakeWidget();
}

FText FSelQueryTestDetails::GetClampMinTypeDesc() const
{
	check(ClampMinTypeHandle.IsValid());
	uint8 EnumValue;
	ClampMinTypeHandle->GetValue(EnumValue);

	for (int32 i = 0; i < ClampMinTypeValues.Num(); i++)
	{
		if (ClampMinTypeValues[i].Int == EnumValue)
		{
			return ClampMinTypeValues[i].Text;
		}
	}

	return FText::GetEmpty();
}

FText FSelQueryTestDetails::GetClampMaxTypeDesc() const
{
	check(ClampMaxTypeHandle.IsValid());
	uint8 EnumValue;
	ClampMaxTypeHandle->GetValue(EnumValue);

	for (int32 i = 0; i < ClampMaxTypeValues.Num(); i++)
	{
		if (ClampMaxTypeValues[i].Int == EnumValue)
		{
			return ClampMaxTypeValues[i].Text;
		}
	}

	return FText::GetEmpty();
}

FText FSelQueryTestDetails::GetEquationValuesDesc() const
{
	check(ScoreEquationHandle.IsValid());
	uint8 EnumValue;
	ScoreEquationHandle->GetValue(EnumValue);

	for (int32 i = 0; i < ScoreEquationValues.Num(); i++)
	{
		if (ScoreEquationValues[i].Int == EnumValue)
		{
			return ScoreEquationValues[i].Text;
		}
	}

	return FText::GetEmpty();
}

TSharedRef<SWidget> FSelQueryTestDetails::OnGetFilterTestContent()
{
	BuildFilterTestValues();
	FMenuBuilder MenuBuilder(true, NULL);

	for (int32 i = 0; i < FilterTestValues.Num(); i++)
	{
		FUIAction ItemAction(FExecuteAction::CreateSP(this, &FSelQueryTestDetails::OnFilterTestChange, FilterTestValues[i].Int));
		MenuBuilder.AddMenuEntry(FilterTestValues[i].Text, TAttribute<FText>(), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

FText FSelQueryTestDetails::GetCurrentFilterTestDesc() const
{
	uint8 EnumValue;
	FilterTypeHandle->GetValue(EnumValue);

	for (int32 i = 0; i < FilterTestValues.Num(); i++)
	{
		if (FilterTestValues[i].Int == EnumValue)
		{
			return FilterTestValues[i].Text;
		}
	}

	return FText::GetEmpty();
}

TSharedRef<SWidget> FSelQueryTestDetails::OnGetEquationValuesContent()
{
	BuildScoreEquationValues();
	FMenuBuilder MenuBuilder(true, NULL);

	for (int32 i = 0; i < ScoreEquationValues.Num(); i++)
	{
		FUIAction ItemAction(FExecuteAction::CreateSP(this, &FSelQueryTestDetails::OnScoreEquationChange, ScoreEquationValues[i].Int));
		MenuBuilder.AddMenuEntry(ScoreEquationValues[i].Text, TAttribute<FText>(), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

FText FSelQueryTestDetails::GetScoreEquationInfo() const
{
	uint8 EnumValue;
	ScoreEquationHandle->GetValue(EnumValue);

	switch (EnumValue)
	{
		case EEnvTestScoreEquation::Linear:
			return LOCTEXT("Linear","Final score = ScoringFactor * NormalizedItemValue");
			break;

		case EEnvTestScoreEquation::Square:
			return LOCTEXT("Square","Final score = ScoringFactor * (NormalizedItemValue * NormalizedItemValue)\nBias towards items with big values.");
			break;

		case EEnvTestScoreEquation::InverseLinear:
			return LOCTEXT("Inverse","Final score = ScoringFactor * (1.0 - NormalizedItemValue)\nBias towards items with values close to zero.  (Linear, but flipped from 1 to 0 rather than 0 to 1.");
			break;

		case EEnvTestScoreEquation::SquareRoot:
			return LOCTEXT("Square root", "Final score = ScoringFactor * Sqrt(NormalizedItemValue)\nNon-linearly bias towards items with big values.");
			break;

		case EEnvTestScoreEquation::Constant:
			return LOCTEXT("Constant", "Final score (for values that 'pass') = ScoringFactor\nNOTE: In this case, the score is normally EITHER the ScoringFactor value or zero.\nThe score will be zero if the Normalized Test Value is zero (or if the test value is false for a boolean query).\nOtherwise, score will be the ScoringFactor.");
			break;

		default:
			break;
	}

	return FText::GetEmpty();
}

EVisibility FSelQueryTestDetails::GetVisibilityOfValueMinForScoreClamping() const
{
	if (GetFloatScoreVisibility() == EVisibility::Visible)
	{
		const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
		if ((MyTestOb != NULL) && (MyTestOb->ClampMinType == ESelQueryTestClamping::FilterThreshold))
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

EVisibility FSelQueryTestDetails::GetVisibilityOfValueMaxForScoreClamping() const
{
	if (GetFloatScoreVisibility() == EVisibility::Visible)
	{
		const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
		if ((MyTestOb != NULL) && (MyTestOb->ClampMaxType == ESelQueryTestClamping::FilterThreshold))
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

EVisibility FSelQueryTestDetails::GetVisibilityOfScoreClampMinimum() const
{
	if (GetFloatScoreVisibility() == EVisibility::Visible)
	{
		const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
		if ((MyTestOb != NULL) && (MyTestOb->ClampMinType == ESelQueryTestClamping::SpecifiedValue))
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

EVisibility FSelQueryTestDetails::GetVisibilityOfScoreClampMaximum() const
{
	if (GetFloatScoreVisibility() == EVisibility::Visible)
	{
		const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
		if ((MyTestOb != NULL) && (MyTestOb->ClampMaxType == ESelQueryTestClamping::SpecifiedValue))
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

EVisibility FSelQueryTestDetails::GetFloatTestVisibility() const
{
	const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
	if ((MyTestOb != NULL) && MyTestOb->GetWorkOnFloatValues())
	{
		return EVisibility::Visible;
	}

	return EVisibility::Collapsed;
}

bool FSelQueryTestDetails::IsFiltering() const
{
	const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
	if (MyTestOb == NULL)
	{
		return false;
	}

	return MyTestOb->IsFiltering();
}

bool FSelQueryTestDetails::IsScoring() const
{
	const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
	if (MyTestOb == NULL)
	{
		return false;
	}

	return MyTestOb->IsScoring();
}

EVisibility FSelQueryTestDetails::GetFloatFilterVisibility() const
{
	if (IsFiltering())
	{
		const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
		if (MyTestOb && MyTestOb->GetWorkOnFloatValues())
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

EVisibility FSelQueryTestDetails::GetScoreVisibility() const
{
	if (IsScoring())
	{
		return EVisibility::Visible;
	}

	return EVisibility::Collapsed;
}

EVisibility FSelQueryTestDetails::GetClampingVisibility() const
{
	if (IsScoring())
	{
		const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
		if (MyTestOb && MyTestOb->GetWorkOnFloatValues())
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

EVisibility FSelQueryTestDetails::GetFloatScoreVisibility() const
{
	if (IsScoring())
	{
		const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
		if (MyTestOb && MyTestOb->GetWorkOnFloatValues())
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

bool FSelQueryTestDetails::UsesFilterMin() const
{
	uint8 EnumValue;
	check(FilterTypeHandle.IsValid());
	FilterTypeHandle->GetValue(EnumValue);

	return ((EnumValue == EEnvTestFilterType::Minimum) || (EnumValue == EEnvTestFilterType::Range));
}

bool FSelQueryTestDetails::UsesFilterMax() const
{
	uint8 EnumValue;
	check(FilterTypeHandle.IsValid());
	FilterTypeHandle->GetValue(EnumValue);

	return ((EnumValue == EEnvTestFilterType::Maximum) || (EnumValue == EEnvTestFilterType::Range));
}

EVisibility FSelQueryTestDetails::GetVisibilityOfFloatValueMin() const
{
	if (IsFiltering())
	{
		uint8 EnumValue;
		FilterTypeHandle->GetValue(EnumValue);

		const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
		if (MyTestOb && MyTestOb->GetWorkOnFloatValues() &&
			((EnumValue == EEnvTestFilterType::Minimum) || (EnumValue == EEnvTestFilterType::Range)))
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

EVisibility FSelQueryTestDetails::GetVisibilityOfFloatValueMax() const
{
	if (IsFiltering())
	{
		uint8 EnumValue;
		FilterTypeHandle->GetValue(EnumValue);

		const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
		if (MyTestOb && MyTestOb->GetWorkOnFloatValues() &&
			((EnumValue == EEnvTestFilterType::Maximum) || (EnumValue == EEnvTestFilterType::Range)))
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

bool FSelQueryTestDetails::IsMatchingBoolValue() const
{
	// TODO: Decide whether this complex function needs to be so complex!  At the moment, if it's not working on floats,
	// it must be working on bools, in which case it MUST be using a Match test.  So probably it could stop much earlier!
	const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
	if (MyTestOb && !MyTestOb->GetWorkOnFloatValues())
	{
		if (FilterTypeHandle.IsValid())
		{
			uint8 EnumValue;
			FilterTypeHandle->GetValue(EnumValue);
			if (EnumValue == EEnvTestFilterType::Match)
			{
				return true;
			}
		}
	}

	return false;
}

EVisibility FSelQueryTestDetails::GetBoolValueVisibilityForScoring() const
{
	if (!IsFiltering())
	{
		if (IsMatchingBoolValue())
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

EVisibility FSelQueryTestDetails::GetBoolValueVisibility() const
{
	if (IsFiltering())
	{
		if (IsMatchingBoolValue())
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

EVisibility FSelQueryTestDetails::GetTestPreviewVisibility() const
{
	const USelQueryTest* MyTestOb = Cast<const USelQueryTest>(MyTest.Get());
	if ((MyTestOb != NULL) && MyTestOb->GetWorkOnFloatValues())
	{
		return EVisibility::Visible;
	}

	return EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE
