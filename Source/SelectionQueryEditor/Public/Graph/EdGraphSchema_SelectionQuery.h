// Copyright 2015-2016 Piperift. All Rights Reserved.
#pragma once

#include "EdGraphSchema_SelectionQuery.generated.h"

/** Action to add a node to the graph */
USTRUCT()
struct  FSQSchemaAction_NewNode : public FEdGraphSchemaAction
{
    GENERATED_USTRUCT_BODY();

    /** Template of node we want to create */
    UPROPERTY()
    class UEdGraphNode* NodeTemplate;


    FSQSchemaAction_NewNode()
        : FEdGraphSchemaAction()
        , NodeTemplate(NULL)
    {}

    FSQSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FString& InToolTip, const int32 InGrouping)
        : FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
        , NodeTemplate(NULL)
    {}

    // FEdGraphSchemaAction interface
    virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
    virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode = true) override;
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    // End of FEdGraphSchemaAction interface

    template <typename NodeType>
    static NodeType* SpawnNodeFromTemplate(class UEdGraph* ParentGraph, NodeType* InTemplateNode, const FVector2D Location, bool bSelectNewNode = true)
    {
        FSQSchemaAction_NewNode Action;
        Action.NodeTemplate = InTemplateNode;

        return Cast<NodeType>(Action.PerformAction(ParentGraph, NULL, Location, bSelectNewNode));
    }
};

UCLASS()
class UEdGraphSchema_SelectionQuery : public UEdGraphSchema {
    GENERATED_UCLASS_BODY()

    virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const;

    // Begin EdGraphSchema interface
    virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
    virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
    virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
    virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
    virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
    virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
    // End EdGraphSchema interface

    void GetActionList(TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions, const UEdGraph* Graph, UEdGraph* OwnerOfTemporaries) const;

private:
    bool ContainsCycles(const UEdGraphPin* A, const UEdGraphPin* B, TArray<FString>& OutCyclePath) const;

};

class SELECTIONQUERYEDITOR_API SQSchemaUtils {
public:
    template<typename T>
    static void AddAction(FString Title, FString Tooltip, TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions, UEdGraph* OwnerOfTemporaries) {
        const FText MenuDesc = FText::FromString(Title);
        const FText Category = FText::FromString(TEXT("Selection Query"));
        TSharedPtr<FSQSchemaAction_NewNode> NewActorNodeAction = AddNewNodeAction(OutActions, Category, MenuDesc, Tooltip);
        T* ActorNode = NewObject<T>(OwnerOfTemporaries);
        NewActorNodeAction->NodeTemplate = ActorNode;
    }

    static TSharedPtr<FSQSchemaAction_NewNode> AddNewNodeAction(TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions, const FText& Category, const FText& MenuDesc, const FString& Tooltip)
    {
        TSharedPtr<FSQSchemaAction_NewNode> NewAction = TSharedPtr<FSQSchemaAction_NewNode>(new FSQSchemaAction_NewNode(Category, MenuDesc, Tooltip, 0));
        OutActions.Add(NewAction);
        return NewAction;
    }

    static TSharedPtr<FSQSchemaAction_NewNode> AddNewNodeAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FString& Tooltip)
    {
        TSharedPtr<FSQSchemaAction_NewNode> NewAction = TSharedPtr<FSQSchemaAction_NewNode>(new FSQSchemaAction_NewNode(Category, MenuDesc, Tooltip, 0));
        ContextMenuBuilder.AddAction(NewAction);

        return NewAction;
    }
};

