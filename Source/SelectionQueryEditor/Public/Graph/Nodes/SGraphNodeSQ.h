// Copyright 2015-2017 Piperift. All Rights Reserved.
#pragma once

#include "Editor/GraphEditor/Private/GraphEditorCommon.h"

/**
 * SGraphNodeSQ
 * Comes from SGraphNodeAI
 */
class SELECTIONQUERYEDITOR_API SGraphNodeSQ : public SGraphNode
{
public:
    SLATE_BEGIN_ARGS(SGraphNodeSQ) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, USQGraphNode* InNode);

    //~ Begin SGraphNode Interface
    virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
    virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
    //~ End SGraphNode Interface

protected:
    uint32 bDragMarkerVisible : 1;

    virtual FText GetDescription() const;
    virtual EVisibility GetDescriptionVisibility() const;

    virtual FText GetPreviewCornerText() const;
    virtual const FSlateBrush* GetNameIcon() const;
};

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