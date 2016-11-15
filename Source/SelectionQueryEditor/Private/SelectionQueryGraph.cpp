// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "SelectionQueryEditorPrivatePCH.h"

//////////////////////////////////////////////////////////////////////////
// SelectionQueryGraph

namespace EQSGraphVersion
{
	const int32 Initial = 0;
	const int32 NestedNodes = 1;
	const int32 CopyPasteOutersBug = 2;
	const int32 BlueprintClasses = 3;

	const int32 Latest = BlueprintClasses;
}

USelectionQueryGraph::USelectionQueryGraph(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Schema = UEdGraphSchema_SelectionQuery::StaticClass();
}

struct FCompareNodeXLocation
{
	FORCEINLINE bool operator()(const UEdGraphPin& A, const UEdGraphPin& B) const
	{
		return A.GetOwningNode()->NodePosX < B.GetOwningNode()->NodePosX;
	}
};

void USelectionQueryGraph::UpdateAsset(int32 UpdateFlags)
{
	if (IsLocked())
	{
		return;
	}

	// let's find root node
	USelectionQueryGraphNode_Root* RootNode = NULL;
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		RootNode = Cast<USelectionQueryGraphNode_Root>(Nodes[Idx]);
		if (RootNode != NULL)
		{
			break;
		}
	}

	USelectionQuery* SelQuery = Cast<USelectionQuery>(GetOuter());
	SelQuery->GetOptionsMutable().Reset();
    //Check that Root is connected to something
	if (RootNode && RootNode->Pins.Num() > 0 && RootNode->Pins[0]->LinkedTo.Num() > 0)
	{
		UEdGraphPin* Out = RootNode->Pins[0];

		// sort connections so that they're organized the same as user can see in the editor
		Out->LinkedTo.Sort(FCompareNodeXLocation());

		for (int32 Idx = 0; Idx < Out->LinkedTo.Num(); Idx++)
		{
			USelectionQueryGraphNode_Composite* CompositeNode = Cast<USelectionQueryGraphNode_Composite>(Out->LinkedTo[Idx]->GetOwningNode());
			if (CompositeNode)
			{
				CompositeNode->UpdateNodeData();

				USQCompositeNode* CompositeInstance = Cast<USQCompositeNode>(CompositeNode->NodeInstance);
				if (CompositeInstance)
				{
					SelQuery->GetOptionsMutable().Add(CompositeInstance);
				}
                //ERROR Tracking
				else if (CompositeInstance == nullptr)
				{
					FString DebugMessage = FString::Printf(TEXT("[%s] UpdateAsset found cmposite node [pin:%d] without an instance!"),
						FPlatformTime::StrTimestamp(), Idx);

					RootNode->LogDebugMessage(DebugMessage);
				}
			}
		}
	}

	RemoveOrphanedNodes();

	// FORT-16508 tracking BEGIN: find corrupted options
	if (RootNode)
	{
		for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
		{
            USelectionQueryGraphNode_Composite* CompositeNode = Cast<USelectionQueryGraphNode_Composite>(Nodes[Idx]);
			if (CompositeNode)
			{
				USQCompositeNode* CompositeInstance = Cast<USQCompositeNode>(CompositeNode->NodeInstance);
				if (CompositeNode->NodeInstance == nullptr || CompositeInstance == nullptr || CompositeInstance->HasAnyFlags(RF_Transient))
				{
					FString DebugMessage = FString::Printf(TEXT("[%s] found corrupted node after RemoveOrphanedNodes! type:instance option:%s instance:%d transient:%d tests:%d"),
						FPlatformTime::StrTimestamp(),
						*GetNameSafe(CompositeNode),
                        CompositeNode->NodeInstance ? (CompositeInstance ? 1 : -1) : 0,
                        CompositeNode->NodeInstance ? (CompositeNode->HasAnyFlags(RF_Transient) ? 1 : 0) : -1,
                        CompositeNode->SubNodes.Num());

					RootNode->LogDebugError(DebugMessage);
				}
			}
		}
	}
	// FORT-16508 tracking END

#if USE_EQS_DEBUGGER
	USelQueryManager::NotifyAssetUpdate(Query);
#endif
}

void USelectionQueryGraph::Initialize()
{
	Super::Initialize();
	
	LockUpdates();
	SpawnMissingNodes();
	CalculateAllWeights();
	UnlockUpdates();
}

void USelectionQueryGraph::OnLoaded()
{
	Super::OnLoaded();
	UpdateDeprecatedGeneratorClasses();
}

void USelectionQueryGraph::CalculateAllWeights()
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
        USelectionQueryGraphNode_Composite* CompositeNode = Cast<USelectionQueryGraphNode_Composite>(Nodes[Idx]);
		if (CompositeNode)
		{
            CompositeNode->CalculateWeights();
		}
	}
}

void USelectionQueryGraph::MarkVersion()
{
	GraphVersion = SQGraphVersion::Latest;
}

void USelectionQueryGraph::UpdateVersion()
{
	if (GraphVersion == SQGraphVersion::Latest)
	{
		return;
	}

	// convert to nested nodes
	if (GraphVersion < SQGraphVersion::NestedNodes)
	{
		UpdateVersion_NestedNodes();
	}

	if (GraphVersion < SQGraphVersion::CopyPasteOutersBug)
	{
		UpdateVersion_FixupOuters();
	}

	if (GraphVersion < SQGraphVersion::BlueprintClasses)
	{
		UpdateVersion_CollectClassData();
	}

	GraphVersion = SQGraphVersion::Latest;
	Modify();
}

void USelectionQueryGraph::UpdateVersion_NestedNodes()
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USelectionQueryGraphNode_Composite* CompositeNode = Cast<USelectionQueryGraphNode_Composite>(Nodes[Idx]);
		if (CompositeNode)
		{
			USelectionQueryGraphNode* Node = CompositeNode;
			while (Node)
			{
                //HERE: Update nested nodes
			}
		}
	}

	for (int32 Idx = Nodes.Num() - 1; Idx >= 0; Idx--)
	{
		USelectionQueryGraphNode_Test* TestNode = Cast<USelectionQueryGraphNode_Test>(Nodes[Idx]);
		if (TestNode)
		{
			TestNode->Pins.Empty();
			Nodes.RemoveAt(Idx);
			continue;
		}

		USelectionQueryGraphNode_Composite* CompositeNode = Cast<USelectionQueryGraphNode_Composite>(Nodes[Idx]);
		if (CompositeNode && CompositeNode->Pins.IsValidIndex(1))
		{
            CompositeNode->Pins[1]->MarkPendingKill();
            CompositeNode->Pins.RemoveAt(1);
		}
	}
}

void USelectionQueryGraph::UpdateVersion_FixupOuters()
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USelectionQueryGraphNode* MyNode = Cast<USelectionQueryGraphNode>(Nodes[Idx]);
		if (MyNode)
		{
			MyNode->PostEditImport();
		}
	}
}

void USelectionQueryGraph::UpdateVersion_CollectClassData()
{
	UpdateClassData();
}

void USelectionQueryGraph::CollectAllNodeInstances(TSet<UObject*>& NodeInstances)
{
	Super::CollectAllNodeInstances(NodeInstances);

	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USelectionQueryGraphNode* MyNode = Cast<USelectionQueryGraphNode>(Nodes[Idx]);
		USQCompositeNode* CompositeInstance = MyNode ? Cast<USQCompositeNode>(MyNode->NodeInstance) : nullptr;
		if (CompositeInstance)
		{
			NodeInstances.Add(CompositeInstance);
		}
	}
}

void USelectionQueryGraph::OnNodeInstanceRemoved(UObject* NodeInstance)
{
	// FORT-16508 tracking BEGIN: log removing a node instance
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USelectionQueryGraphNode_Root* RootNode = Cast<USelectionQueryGraphNode_Root>(Nodes[Idx]);
		if (RootNode)
		{
			FString DebugMessage = FString::Printf(TEXT("[%s] RemoveInstance %s owner:%s wasTransient:%d"),
				FPlatformTime::StrTimestamp(),
				*GetNameSafe(NodeInstance),
				NodeInstance ? *GetNameSafe(NodeInstance->GetOuter()) : TEXT("??"),
				NodeInstance ? (NodeInstance->HasAnyFlags(RF_Transient) ? 1 : 0) : -1);

			RootNode->LogDebugMessage(DebugMessage);
		}
	}
	// FORT-16508 tracking END
}

void USelectionQueryGraph::OnNodesPasted(const FString& ImportStr)
{
	// FORT-16508 tracking BEGIN: log removing a node instance
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USelectionQueryGraphNode_Root* RootNode = Cast<USelectionQueryGraphNode_Root>(Nodes[Idx]);
		if (RootNode)
		{
			FString DebugMessage = FString::Printf(TEXT("[%s] PasteNodes\n\n%s"),
				FPlatformTime::StrTimestamp(), *ImportStr);

			RootNode->LogDebugMessage(DebugMessage);
		}
	}
	// FORT-16508 tracking END
}

void USelectionQueryGraph::UpdateDeprecatedGeneratorClasses()
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USelectionQueryGraphNode* MyNode = Cast<USelectionQueryGraphNode>(Nodes[Idx]);
		USQCompositeNode* CompositeInstance = MyNode ? Cast<USQCompositeNode>(MyNode->NodeInstance) : nullptr;
		if (CompositeInstance)
		{
			MyNode->ErrorMessage = FGraphNodeClassHelper::GetDeprecationMessage(CompositeInstance->GetClass());
		}
	}
}

void USelectionQueryGraph::SpawnMissingNodes()
{
	USelectionQuery* QueryOwner = Cast<USelectionQuery>(GetOuter());
	if (QueryOwner == nullptr)
	{
		return;
	}

	TSet<USQCompositeNode*> ExistingNodes;
	TArray<USQCompositeNode*> CompositesCopy = QueryOwner->GetOptions();

	UAIGraphNode* MyRootNode = nullptr;
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USelectionQueryGraphNode* MyNode = Cast<USelectionQueryGraphNode>(Nodes[Idx]);
		USQCompositeNode* CompositeInstance = MyNode ? Cast<USQCompositeNode>(MyNode->NodeInstance) : nullptr;
		if (CompositeInstance)
		{
			ExistingNodes.Add(CompositeInstance);

			//SpawnMissingSubNodes(CompositeInstance, ExistingTests, MyNode);
		}

		USelectionQueryGraphNode_Root* RootNode = Cast<USelectionQueryGraphNode_Root>(Nodes[Idx]);
		if (RootNode)
		{
			MyRootNode = RootNode;
		}
	}

	UEdGraphPin* RootOutPin = MyRootNode ? FindGraphNodePin(MyRootNode, EGPD_Output) : nullptr;
	ExistingTests.Empty(0);

	for (int32 Idx = 0; Idx < CompositesCopy.Num(); Idx++)
	{
		USQCompositeNode* CompositeInstance = CompositesCopy[Idx];
		if (ExistingNodes.Contains(CompositeInstance) || CompositeInstance == nullptr)
		{
			continue;
		}

		FGraphNodeCreator<USelectionQueryGraphNode_Composite> NodeBuilder(*this);
		USelectionQueryGraphNode_Composite* MyNode = NodeBuilder.CreateNode();
		UAIGraphNode::UpdateNodeClassDataFrom(CompositeInstance->GetClass(), MyNode->ClassData);
		MyNode->ErrorMessage = MyNode->ClassData.GetDeprecatedMessage();
		NodeBuilder.Finalize();

		if (MyRootNode)
		{
			MyNode->NodePosX = MyRootNode->NodePosX + (Idx * 300);
			MyNode->NodePosY = MyRootNode->NodePosY + 100;
		}

		MyNode->NodeInstance = CompositeInstance;
		//SpawnMissingSubNodes(CompositeInstance, ExistingTests, MyNode);

		UEdGraphPin* SpawnedInPin = FindGraphNodePin(MyNode, EGPD_Input);
		if (RootOutPin && SpawnedInPin)
		{
			RootOutPin->MakeLinkTo(SpawnedInPin);
		}
	}
}

/*
void USelectionQueryGraph::SpawnMissingSubNodes(USelQueryOption* Option, TSet<USelQueryTest*> ExistingTests, USelectionQueryGraphNode* OptionNode)
{
	TArray<USelQueryTest*> TestsCopy = Option->Tests;
	for (int32 SubIdx = 0; SubIdx < TestsCopy.Num(); SubIdx++)
	{
		if (ExistingTests.Contains(TestsCopy[SubIdx]) || (TestsCopy[SubIdx] == nullptr))
		{
			continue;
		}

		USelectionQueryGraphNode_Test* TestNode = NewObject<USelectionQueryGraphNode_Test>(this);
		TestNode->NodeInstance = TestsCopy[SubIdx];
		TestNode->UpdateNodeClassData();

		OptionNode->AddSubNode(TestNode, this);
		TestNode->NodeInstance = TestsCopy[SubIdx];
	}
}*/
