// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "SelTraceDataCustomization.h"
#include "SelectionQuery/SelectionQueryTypes.h"

#define LOCTEXT_NAMESPACE "SelTraceDataCustomization"

TSharedRef<IPropertyTypeCustomization> FSelTraceDataCustomization::MakeInstance()
{
	return MakeShareable( new FSelTraceDataCustomization );
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FSelTraceDataCustomization::CustomizeHeader( TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{
	HeaderRow.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.VAlign(VAlign_Center)
	[
		SNew(STextBlock)
		.Text(this, &FSelTraceDataCustomization::GetShortDescription)
		.Font(IDetailLayoutBuilder::GetDetailFont())
	];

	PropTraceMode = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData,TraceMode));
	PropTraceShape = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData,TraceShape));
	PropTraceChannel = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData, TraceChannel));
	PropTraceChannelSerialized = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData, SerializedChannel));
	CacheTraceModes(StructPropertyHandle);
}

void FSelTraceDataCustomization::CustomizeChildren( TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{
	if (TraceModes.Num() > 1)
	{
		StructBuilder.AddChildContent(LOCTEXT("TraceMode", "Trace Mode"))
		.NameContent()
		[
			PropTraceMode->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SComboButton)
			.OnGetMenuContent(this, &FSelTraceDataCustomization::OnGetTraceModeContent)
			.ContentPadding(FMargin( 2.0f, 2.0f ))
			.ButtonContent()
			[
				SNew(STextBlock) 
				.Text(this, &FSelTraceDataCustomization::GetCurrentTraceModeDesc)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];
	}

	// navmesh props
	TSharedPtr<IPropertyHandle> PropNavFilter = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData,NavigationFilter));
	StructBuilder.AddChildProperty(PropNavFilter.ToSharedRef())
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelTraceDataCustomization::GetNavigationVisibility)));

	// geometry props
	PropTraceChannel->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FSelTraceDataCustomization::OnTraceChannelChanged));
	StructBuilder.AddChildProperty(PropTraceChannel.ToSharedRef())
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelTraceDataCustomization::GetGeometryVisibility)));

	StructBuilder.AddChildProperty(PropTraceShape.ToSharedRef())
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelTraceDataCustomization::GetGeometryVisibility)));

	// common props
	TSharedPtr<IPropertyHandle> PropExtX = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData,ExtentX));
	StructBuilder.AddChildProperty(PropExtX.ToSharedRef())
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelTraceDataCustomization::GetExtentX)));

	TSharedPtr<IPropertyHandle> PropExtY = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData,ExtentY));
	StructBuilder.AddChildProperty(PropExtY.ToSharedRef())
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelTraceDataCustomization::GetExtentY)));

	TSharedPtr<IPropertyHandle> PropExtZ = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData,ExtentZ));
	StructBuilder.AddChildProperty(PropExtZ.ToSharedRef())
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelTraceDataCustomization::GetExtentZ)));

	// projection props
	TSharedPtr<IPropertyHandle> PropHeightDown = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData,ProjectDown));
	StructBuilder.AddChildProperty(PropHeightDown.ToSharedRef())
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelTraceDataCustomization::GetProjectionVisibility)));

	TSharedPtr<IPropertyHandle> PropHeightUp = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData,ProjectUp));
	StructBuilder.AddChildProperty(PropHeightUp.ToSharedRef())
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelTraceDataCustomization::GetProjectionVisibility)));

	// advanced props
	TSharedPtr<IPropertyHandle> PropPostProjectionVerticalOffset = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData, PostProjectionVerticalOffset));
	StructBuilder.AddChildProperty(PropPostProjectionVerticalOffset.ToSharedRef())
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelTraceDataCustomization::GetProjectionVisibility)));

	TSharedPtr<IPropertyHandle> PropTraceComplex = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData,bTraceComplex));
	StructBuilder.AddChildProperty(PropTraceComplex.ToSharedRef())
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelTraceDataCustomization::GetGeometryVisibility)));

	TSharedPtr<IPropertyHandle> PropOnlyBlocking = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData,bOnlyBlockingHits));
	StructBuilder.AddChildProperty(PropOnlyBlocking.ToSharedRef())
		.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelTraceDataCustomization::GetGeometryVisibility)));
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FSelTraceDataCustomization::CacheTraceModes(TSharedRef<class IPropertyHandle> StructPropertyHandle)
{
	TSharedPtr<IPropertyHandle> PropCanNavMesh = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData, bCanTraceOnNavMesh));
	TSharedPtr<IPropertyHandle> PropCanGeometry = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData, bCanTraceOnGeometry));
	TSharedPtr<IPropertyHandle> PropCanDisable = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData, bCanDisableTrace));
	TSharedPtr<IPropertyHandle> PropCanProject = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelTraceData, bCanProjectDown));

	bool bCanNavMesh = false;
	bool bCanGeometry = false;
	bool bCanDisable = false;
	bCanShowProjection = false;
	PropCanNavMesh->GetValue(bCanNavMesh);
	PropCanGeometry->GetValue(bCanGeometry);
	PropCanDisable->GetValue(bCanDisable);
	PropCanProject->GetValue(bCanShowProjection);

	static UEnum* TraceModeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESelQueryTrace"));
	check(TraceModeEnum);

	TraceModes.Reset();
	if (bCanDisable)
	{
		TraceModes.Add(FTextIntPair(TraceModeEnum->GetEnumText(ESelQueryTrace::None), ESelQueryTrace::None));
	}
	if (bCanNavMesh)
	{
		TraceModes.Add(FTextIntPair(TraceModeEnum->GetEnumText(ESelQueryTrace::Navigation), ESelQueryTrace::Navigation));
	}
	if (bCanGeometry)
	{
		TraceModes.Add(FTextIntPair(TraceModeEnum->GetEnumText(ESelQueryTrace::Geometry), ESelQueryTrace::Geometry));
	}

	ActiveMode = ESelQueryTrace::None;
	PropTraceMode->GetValue(ActiveMode);
}

void FSelTraceDataCustomization::OnTraceChannelChanged()
{
	uint8 TraceChannelValue;
	FPropertyAccess::Result Result = PropTraceChannel->GetValue(TraceChannelValue);
	if (Result == FPropertyAccess::Success)
	{
		ETraceTypeQuery TraceTypeValue = (ETraceTypeQuery)TraceChannelValue;
		ECollisionChannel CollisionChannelValue = UEngineTypes::ConvertToCollisionChannel(TraceTypeValue);

		uint8 SerializedChannelValue = CollisionChannelValue;
		PropTraceChannelSerialized->SetValue(SerializedChannelValue);
	}
}

void FSelTraceDataCustomization::OnTraceModeChanged(int32 Index)
{
	ActiveMode = Index;
	PropTraceMode->SetValue(ActiveMode);
}

TSharedRef<SWidget> FSelTraceDataCustomization::OnGetTraceModeContent()
{
	FMenuBuilder MenuBuilder(true, NULL);

	for (int32 i = 0; i < TraceModes.Num(); i++)
	{
		FUIAction ItemAction( FExecuteAction::CreateSP( this, &FSelTraceDataCustomization::OnTraceModeChanged, TraceModes[i].Int ) );
		MenuBuilder.AddMenuEntry( TraceModes[i].Text, TAttribute<FText>(), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

FText FSelTraceDataCustomization::GetCurrentTraceModeDesc() const
{
	for (int32 i = 0; i < TraceModes.Num(); i++)
	{
		if (TraceModes[i].Int == ActiveMode)
		{
			return TraceModes[i].Text;
		}
	}

	return FText::GetEmpty();
}

FText FSelTraceDataCustomization::GetShortDescription() const
{
	FText Desc = FText::GetEmpty();

	switch (ActiveMode)
	{
	case ESelQueryTrace::Geometry:
		Desc = LOCTEXT("TraceGeom","geometry trace");
		break;

	case ESelQueryTrace::Navigation:
		Desc = LOCTEXT("TraceNav","navmesh trace");
		break;

	case ESelQueryTrace::None:
		Desc = LOCTEXT("TraceNone","trace disabled");
		break;

	default: break;
	}

	return Desc;
}

EVisibility FSelTraceDataCustomization::GetGeometryVisibility() const
{
	return (ActiveMode == ESelQueryTrace::Geometry) ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FSelTraceDataCustomization::GetNavigationVisibility() const
{
	return (ActiveMode == ESelQueryTrace::Navigation) ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FSelTraceDataCustomization::GetProjectionVisibility() const
{
	return (ActiveMode != ESelQueryTrace::None) && bCanShowProjection ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FSelTraceDataCustomization::GetExtentX() const
{
	if (ActiveMode == ESelQueryTrace::Navigation)
	{
		// radius
		return EVisibility::Visible;
	}
	else if (ActiveMode == ESelQueryTrace::Geometry)
	{
		uint8 EnumValue;
		PropTraceShape->GetValue(EnumValue);

		return (EnumValue != ESelTraceShape::Line) ? EVisibility::Visible : EVisibility::Collapsed;
	}
		
	return EVisibility::Collapsed;
}

EVisibility FSelTraceDataCustomization::GetExtentY() const
{
	if (ActiveMode == ESelQueryTrace::Geometry)
	{
		uint8 EnumValue;
		PropTraceShape->GetValue(EnumValue);

		return (EnumValue == ESelTraceShape::Box) ? EVisibility::Visible : EVisibility::Collapsed;
	}

	return EVisibility::Collapsed;
}

EVisibility FSelTraceDataCustomization::GetExtentZ() const
{
	if (ActiveMode == ESelQueryTrace::Geometry)
	{
		uint8 EnumValue;
		PropTraceShape->GetValue(EnumValue);

		return (EnumValue == ESelTraceShape::Box || EnumValue == ESelTraceShape::Capsule) ? EVisibility::Visible : EVisibility::Collapsed;
	}

	return EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE
