// Copyright 2015-2016 Piperift. All Rights Reserved.
#pragma once

#include "IPropertyTypeCustomization.h"
#include "StringEnumCustomization.h"

class FAnchorTypeCustomization : public FStringEnumCustomization
{
public:
	/**
	* Creates a new instance.
	*
	* @return A new struct customization for Anchor Type.
	*/
    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FAnchorTypeCustomization);
	}

private:
	/** Handle to the struct properties being customized */
	TSharedPtr<IPropertyHandle> StructHandle;
	TSharedPtr<IPropertyHandle> TypeHandle;

	/** Update the root data on a change of selection */
	//void OnSettingsChanged();

	virtual bool CanCustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	/** Returns all the enum items.
	 * This is for override purposes.
	 */
	virtual TArray<FString> GetEnumItems() override;

	/** Returns all the enum items.
	 * This is for override purposes.
	 */
	virtual void OnItemSelected(FString Value) override;

	virtual FText GetSelectedItem() const override;
};

