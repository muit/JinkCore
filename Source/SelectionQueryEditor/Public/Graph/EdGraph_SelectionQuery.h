// Copyright 2015-2017 Piperift. All Rights Reserved.
#pragma once

#include "EdGraph_SelectionQuery.generated.h"

struct FSelectionQueryDataTypes {
    static const FString PinCategory_MultipleNodes;
    static const FString PinCategory_SingleComposite;
    static const FString PinCategory_SingleItem;
};

struct FSQGraphBuildError {
    //class USQGraphNode* NodeWithError;
    FString ErrorMessage;
    FString ErrorTooltip;
};

UCLASS()
class UEdGraph_SelectionQuery : public UEdGraph {
    GENERATED_UCLASS_BODY()

public:
    void RebuildGraph(USelectionQuery* DataAsset, /*TArray<FPropTypeData>& OutProps, */TArray<FSQGraphBuildError>& OutErrors);

    void InitializeGraph();

    /** Add a listener for OnGraphChanged events */
    FDelegateHandle AddOnNodePropertyChangedHandler(const FOnGraphChanged::FDelegate& InHandler);
    /** Remove a listener for OnGraphChanged events */
    void RemoveOnNodePropertyChangedHandler(FDelegateHandle Handle);
    /** Notify OnGraphChanged events */
    void NotifyNodePropertyChanged(const FEdGraphEditAction& InAction);

private:
    /** A delegate that broadcasts a notification whenever the graph has changed. */
    FOnGraphChanged OnNodePropertyChanged;
};
