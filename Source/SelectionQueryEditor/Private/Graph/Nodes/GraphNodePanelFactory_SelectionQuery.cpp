// Copyright 2015-2017 Piperift. All Rights Reserved.
#include "SelectionQueryEditorPrivatePCH.h"
#include "Graph/Nodes/GraphNodePanelFactory_SelectionQuery.h"
#include "SGraphNode_SQItem.h"
#include "SGraphNodeSQ.h"
#include "Graph/SQGraphNode_Item.h"

FGraphPanelNodeFactory_SelectionQuery::FGraphPanelNodeFactory_SelectionQuery()
{
}

TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_SelectionQuery::CreateNode(UEdGraphNode* Node) const
{
	if (USQGraphNode_Item* ActorNode = Cast<USQGraphNode_Item>(Node))
	{
		TSharedPtr<SGraphNode_SQItem> SNode = SNew(SGraphNode_SQItem, ActorNode);
		ActorNode->PropertyObserver = SNode;
		return SNode;
	}
	/*else if (USQGraphNode_Composite* CompositeNode = Cast<USQGraphNode_Composite>(Node))
	{
		TSharedPtr<SGraphNode_SQComposite> SNode = SNew(SGraphNode_SQComposite, CompositeNode);
		CompositeNode->PropertyObserver = SNode;
		return SNode;
	}
	else if (USQGraphNode_Root* RootNode = Cast<USQGraphNode_Root>(Node))
	{
		TSharedPtr<SGraphNode_SQRoot> SNode = SNew(SGraphNode_SQRoot, RootNode);
		RootNode->PropertyObserver = SNode;
		return SNode;
	}*/

    return FGraphPanelNodeFactory::CreateNode(Node);
}
