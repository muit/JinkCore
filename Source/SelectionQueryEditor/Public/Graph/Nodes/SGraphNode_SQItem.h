// Copyright 2015-2017 Piperift. All Rights Reserved.
#pragma once

#include "SGraphNode.h"
#include "AssetThumbnail.h"

/**
 * Implements the message interaction graph panel.
 */
class SELECTIONQUERYEDITOR_API SGraphNode_SQItem : public SGraphNode, public FNodePropertyObserver
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_SQItem) { }
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, USQGraphNode_Item* InNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual TArray<FOverlayWidgetInfo> GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const override;
	// End of SGraphNode interface

	// SNodePanel::SNode interface
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter) override;

	// End of SNodePanel::SNode interface

	void RebuildExecutionOrder();

	// FPropertyObserver interface
	virtual void OnPropertyChanged(USQGraphNode* Sender, const FName& PropertyName) override;
	// End of FPropertyObserver interface

	struct FObjectOrAssetData
	{
		UObject* Object;
		FAssetData AssetData;

		FObjectOrAssetData(UObject* InObject = NULL)
			: Object(InObject)
		{
			AssetData = InObject != NULL && !InObject->IsA<AActor>() ? FAssetData(InObject) : FAssetData();
		}

		FObjectOrAssetData(const FAssetData& InAssetData)
			: Object(NULL)
			, AssetData(InAssetData)
		{}

		bool IsValid() const
		{
			return Object != NULL || AssetData.IsValid();
		}
	};

private:
	void SetThumbnail(UObject* AssetObject);
	void BuildThumbnailWidget();
	bool GetValue(FObjectOrAssetData& OutValue) const;

	/** Get the visibility of the index overlay */
	EVisibility GetIndexVisibility() const;

	/** Get the text to display in the index overlay */
	FText GetIndexText() const;

	/** Get the tooltip for the index overlay */
	FText GetIndexTooltipText() const;

	/** Get the color to display for the index overlay. This changes on hover state of sibling nodes */
	FSlateColor GetIndexColor(bool bHovered) const;

	/** Handle hover state changing for the index widget - we use this to highlight sibling nodes */
	void OnIndexHoverStateChanged(bool bHovered);

protected:
    virtual FText GetDescription() const;
    virtual EVisibility GetDescriptionVisibility() const;

	FSlateColor GetBorderBackgroundColor() const;
	virtual const FSlateBrush* GetNameIcon() const;

	USQGraphNode_Item* EdActorNode;
	FIntPoint ThumbnailSize;

    /** The node body widget, cached here so we can determine its size when we want ot position our overlays */
    TSharedPtr<SBorder> NodeBody;

	TSharedPtr<SHorizontalBox> OutputPinBox;
	/** Thumbnail for the asset */
	TSharedPtr<class FAssetThumbnail> AssetThumbnail;

	/** Cached data */
	mutable FAssetData CachedAssetData;

	/** The widget we use to display the index of the node */
	TSharedPtr<SWidget> IndexOverlay;
};