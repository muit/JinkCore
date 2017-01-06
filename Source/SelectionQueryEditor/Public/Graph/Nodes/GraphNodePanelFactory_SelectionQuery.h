// Copyright 2015-2017 Piperift. All Rights Reserved.
#pragma once
#include "EdGraphUtilities.h"

class SELECTIONQUERYEDITOR_API FGraphPanelNodeFactory_SelectionQuery : public FGraphPanelNodeFactory
{
public:
	FGraphPanelNodeFactory_SelectionQuery();

private:
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
