// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SelectionQueryEditorPrivatePCH.h"
#include "SelDirectionCustomization.h"
#include "SelectionQuery/SelectionQueryTypes.h"

#define LOCTEXT_NAMESPACE "FEnvQueryCustomization"

TSharedRef<IPropertyTypeCustomization> FSelDirectionCustomization::MakeInstance( )
{
	return MakeShareable(new FSelDirectionCustomization);
}

void FSelDirectionCustomization::CustomizeHeader( TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{
	// create struct header
	HeaderRow.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.VAlign(VAlign_Center)
	[
		SNew(STextBlock)
		.Text(this, &FSelDirectionCustomization::GetShortDescription)
		.Font(IDetailLayoutBuilder::GetDetailFont())
	];

	ModeProp = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelDirection,DirMode));
	if (ModeProp.IsValid())
	{
		FSimpleDelegate OnModeChangedDelegate = FSimpleDelegate::CreateSP(this, &FSelDirectionCustomization::OnModeChanged);
		ModeProp->SetOnPropertyValueChanged(OnModeChangedDelegate);
	}
	
	OnModeChanged();
}

void FSelDirectionCustomization::CustomizeChildren( TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{
	StructBuilder.AddChildProperty(ModeProp.ToSharedRef());

	TSharedPtr<IPropertyHandle> PropFrom = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelDirection,LineFrom));
	StructBuilder.AddChildProperty(PropFrom.ToSharedRef())
	.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelDirectionCustomization::GetTwoPointsVisibility)));

	TSharedPtr<IPropertyHandle> PropTo = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelDirection,LineTo));
	StructBuilder.AddChildProperty(PropTo.ToSharedRef())
	.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelDirectionCustomization::GetTwoPointsVisibility)));

	TSharedPtr<IPropertyHandle> PropRot = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSelDirection,Rotation));
	StructBuilder.AddChildProperty(PropRot.ToSharedRef())
	.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FSelDirectionCustomization::GetRotationVisibility)));
}

FText FSelDirectionCustomization::GetShortDescription() const
{
	return bIsRotation ? LOCTEXT("DirectionShortDescRotatation", "context's rotation...") : LOCTEXT("DirectionShortDescBetweenTwoPoints", "between two contexts...");
}

EVisibility FSelDirectionCustomization::GetTwoPointsVisibility() const
{
	return bIsRotation ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FSelDirectionCustomization::GetRotationVisibility() const
{
	return bIsRotation ? EVisibility::Visible : EVisibility::Collapsed;
}

void FSelDirectionCustomization::OnModeChanged()
{
	uint8 EnumValue = 0;
	ModeProp->GetValue(EnumValue);

	bIsRotation = (EnumValue == ESelDirection::Rotation);
}

#undef LOCTEXT_NAMESPACE
