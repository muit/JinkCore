// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ISelectionQueryEditor.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Toolkits/AssetEditorManager.h"
#include "IDetailsView.h"
#include "Tickable.h"

class USelectionQuery;

struct FSelectionQueryEditorTabs
{
    // Tab identifiers
    static const FName UpdateGraphId;
    static const FName PropertiesId;
};

class USelectionQuery;

class FSelectionQueryEditor : public ISelectionQueryEditor, public FNotifyHook, public FTickableGameObject
{
public:
    ~FSelectionQueryEditor();

    // Begin IToolkit interface
    virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
    // End IToolkit interface

    // Begin FAssetEditorToolkit interface
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual FText GetToolkitName() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    // End FAssetEditorToolkit interface

    // Begin FTickableGameObject Interface
    virtual bool IsTickableInEditor() const override { return true; }
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override { return TStatId(); }
    // End FTickableGameObject Interface

    void InitSelectionQueryEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, USelectionQuery* InScript);

    FORCEINLINE TSharedPtr<SGraphEditor> GetGraphEditor() const { return GraphEditor; }

    void ShowObjectDetails(UObject* ObjectProperties);

protected:
    void ExtendMenu() {}
    TSharedRef<class SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph);

    /** Select every node in the graph */
    void SelectAllNodes();
    /** Whether we can select every node */
    bool CanSelectAllNodes() const;

    /** Deletes all the selected nodes */
    void DeleteSelectedNodes();

    bool CanDeleteNode(class UEdGraphNode* Node);

    /** Delete an array of Material Graph Nodes and their corresponding expressions/comments */
    void DeleteNodes(const TArray<class UEdGraphNode*>& NodesToDelete);

    /** Delete only the currently selected nodes that can be duplicated */
    void DeleteSelectedDuplicatableNodes();

    /** Whether we are able to delete the currently selected nodes */
    bool CanDeleteNodes() const;

    /** Copy the currently selected nodes */
    void CopySelectedNodes();
    /** Whether we are able to copy the currently selected nodes */
    bool CanCopyNodes() const;

    /** Paste the contents of the clipboard */
    void PasteNodes();
    virtual bool CanPasteNodes() const;
    virtual void PasteNodesHere(const FVector2D& Location);

    /** Cut the currently selected nodes */
    void CutSelectedNodes();
    /** Whether we are able to cut the currently selected nodes */
    bool CanCutNodes() const;

    /** Duplicate the currently selected nodes */
    void DuplicateNodes();
    /** Whether we are able to duplicate the currently selected nodes */
    bool CanDuplicateNodes() const;

    void OnGraphChanged(const FEdGraphEditAction& Action) {
        bGraphStateChanged = true;
    }

    void OnNodePropertyChanged(const FEdGraphEditAction& Action);

    void HandleGraphChanged();

protected:
    void CreateInternalWidgets();

    /** Called when "Save" is clicked for this asset */
    virtual void SaveAsset_Execute() override;

    /** Called when the selection changes in the GraphEditor */
    void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);
    

    /** Called when a node is double clicked */
    void OnNodeDoubleClicked(class UEdGraphNode* Node);

protected:
    /** Currently focused graph */
    TSharedPtr<SGraphEditor> GraphEditor;
    /** The command list for this editor */
    TSharedPtr<FUICommandList> GraphEditorCommands;
    TSharedPtr<IDetailsView> DetailsView;


    USelectionQuery* Query;

    TSharedRef<SDockTab> SpawnTab_UpdateGraph(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_Properties(const FSpawnTabArgs& Args);


    /** Palette of Node actions to perform on the graph */
    TSharedPtr<class SGraphPalette_PropActions> ActionPalette;

    /** Preview Viewport widget */
    //TSharedPtr<class SSelectionQueryEditorViewport> PreviewViewport;

    /** Handle to the registered OnGraphChanged delegate. */
    FDelegateHandle OnGraphChangedDelegateHandle;

    /** Handle to the registered OnNodePropertyChanged delegate. */
    FDelegateHandle OnNodePropertyChangedDelegateHandle;

    bool bGraphStateChanged;
};