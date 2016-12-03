// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ISelectionQueryEditor.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "EdGraph/EdGraphEditor.h"

//class USelectionQuery;

class FSelectionQueryEditor : public ISelectionQueryEditor, public FEdGraphEditor
{
public:

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	void InitSelectionQueryEditor( const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, USelectionQuery* Script );

	//~ Begin IToolkit Interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	//~ End IToolkit Interface

	//~ Begin ISelectionQueryEditor Interface
	virtual uint32 GetSelectedNodesCount() const override { return SelectedNodesCount; }
	//~ End ISelectionQueryEditor Interface

	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);

protected:
	/** Called when "Save" is clicked for this asset */
	virtual void SaveAsset_Execute() override;

private:
	/** Create widget for graph editing */
	TSharedRef<class SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph);

	/** Creates all internal widgets for the tabs to point at */
	void CreateInternalWidgets();

	/** Called when the selection changes in the GraphEditor */
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection) override;

	/** Spawns the tab with the update graph inside */
	TSharedRef<SDockTab> SpawnTab_UpdateGraph(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Properties(const FSpawnTabArgs& Args);
private:

	/* Query being edited */
	USelectionQuery* Query;

	/** Property View */
	TSharedPtr<class IDetailsView> DetailsView;

	uint32 SelectedNodesCount;

	/**	Graph editor tab */
	static const FName SQUpdateGraphTabId;
	static const FName SQPropertiesTabId;
};
