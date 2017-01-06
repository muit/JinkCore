// Copyright 2015-2017 Piperift. All Rights Reserved.
#pragma once

#include "SGraphPin.h"

class SELECTIONQUERYEDITOR_API SSelectionQueryPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SSelectionQueryPin){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
protected:
	//~ Begin SGraphPin Interface
	virtual FSlateColor GetPinColor() const override;
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	//~ End SGraphPin Interface

	const FSlateBrush* GetPinBorder() const;
};