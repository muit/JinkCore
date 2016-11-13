// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "SelectionQueryEditorPrivatePCH.h"
#include "SelectionQueryEditorModule.h"

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

	USelectionQuery* Query = Cast<USeelctionQuery>(GetOuter());
	Query->GetOptionsMutable().Reset();
	if (RootNode && RootNode->Pins.Num() > 0 && RootNode->Pins[0]->LinkedTo.Num() > 0)
	{
		UEdGraphPin* MyPin = RootNode->Pins[0];

		// sort connections so that they're organized the same as user can see in the editor
		MyPin->LinkedTo.Sort(FCompareNodeXLocation());

		for (int32 Idx = 0; Idx < MyPin->LinkedTo.Num(); Idx++)
		{
			USelectionQueryGraphNode_Option* OptionNode = Cast<USelectionQueryGraphNode_Option>(MyPin->LinkedTo[Idx]->GetOwningNode());
			if (OptionNode)
			{
				OptionNode->UpdateNodeData();

				USelQueryOption* OptionInstance = Cast<USelQueryOption>(OptionNode->NodeInstance);
				if (OptionInstance && OptionInstance->Generator)
				{
					OptionInstance->Tests.Reset();

					for (int32 TestIdx = 0; TestIdx < OptionNode->SubNodes.Num(); TestIdx++)
					{
						UAIGraphNode* SubNode = OptionNode->SubNodes[TestIdx];
						if (SubNode == nullptr)
						{
							continue;
						}

						SubNode->ParentNode = OptionNode;

						USelectionQueryGraphNode_Test* TestNode = Cast<USelectionQueryGraphNode_Test>(SubNode);
						if (TestNode && TestNode->bTestEnabled)
						{
							USelQueryTest* TestInstance = Cast<USelQueryTest>(TestNode->NodeInstance);
							if (TestInstance)
							{
								OptionInstance->Tests.Add(TestInstance);
							}
						}
					}

					Query->GetOptionsMutable().Add(OptionInstance);
				}
				
				// FORT-16508 tracking BEGIN: log invalid option
				if (OptionInstance && OptionInstance->Generator == nullptr)
				{
					FString DebugMessage = FString::Printf(TEXT("[%s] UpdateAsset found option instance [pin:%d] without a generator! tests:%d"),
						FPlatformTime::StrTimestamp(), Idx, OptionNode->SubNodes.Num());

					RootNode->LogDebugMessage(DebugMessage);
				}
				else if (OptionInstance == nullptr)
				{
					FString DebugMessage = FString::Printf(TEXT("[%s] UpdateAsset found option node [pin:%d] without an instance! tests:%d"),
						FPlatformTime::StrTimestamp(), Idx, OptionNode->SubNodes.Num());

					RootNode->LogDebugMessage(DebugMessage);
				}
				// FORT-16508 tracking END
			}
		}
	}

	RemoveOrphanedNodes();

	// FORT-16508 tracking BEGIN: find corrupted options
	if (RootNode)
	{
		for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
		{
			USelectionQueryGraphNode_Option* OptionNode = Cast<USelectionQueryGraphNode_Option>(Nodes[Idx]);
			if (OptionNode)
			{
				USelQueryOption* OptionInstance = Cast<USelQueryOption>(OptionNode->NodeInstance);
				if (OptionNode->NodeInstance == nullptr || OptionInstance == nullptr || OptionInstance->HasAnyFlags(RF_Transient))
				{
					FString DebugMessage = FString::Printf(TEXT("[%s] found corrupted node after RemoveOrphanedNodes! type:instance option:%s instance:%d transient:%d tests:%d"),
						FPlatformTime::StrTimestamp(),
						*GetNameSafe(OptionNode),
						OptionNode->NodeInstance ? (OptionInstance ? 1 : -1) : 0,
						OptionNode->NodeInstance ? (OptionNode->HasAnyFlags(RF_Transient) ? 1 : 0) : -1,
						OptionNode->SubNodes.Num());					

					RootNode->LogDebugError(DebugMessage);
				}

				if (OptionInstance && (OptionInstance->Generator == nullptr || OptionInstance->Generator->HasAnyFlags(RF_Transient)))
				{
					FString DebugMessage = FString::Printf(TEXT("[%s] found corrupted node after RemoveOrphanedNodes! type:generator option:%s instance:%d transient:%d tests:%d"),
						FPlatformTime::StrTimestamp(),
						*GetNameSafe(OptionNode),
						OptionNode->NodeInstance ? 1 : 0,
						OptionNode->NodeInstance ? (OptionNode->HasAnyFlags(RF_Transient) ? 1 : 0) : -1,
						OptionNode->SubNodes.Num());

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
		USelectionQueryGraphNode_Option* OptionNode = Cast<USelectionQueryGraphNode_Option>(Nodes[Idx]);
		if (OptionNode)
		{
			OptionNode->CalculateWeights();
		}
	}
}

void USelectionQueryGraph::MarkVersion()
{
	GraphVersion = EQSGraphVersion::Latest;
}

void USelectionQueryGraph::UpdateVersion()
{
	if (GraphVersion == EQSGraphVersion::Latest)
	{
		return;
	}

	// convert to nested nodes
	if (GraphVersion < EQSGraphVersion::NestedNodes)
	{
		UpdateVersion_NestedNodes();
	}

	if (GraphVersion < EQSGraphVersion::CopyPasteOutersBug)
	{
		UpdateVersion_FixupOuters();
	}

	if (GraphVersion < EQSGraphVersion::BlueprintClasses)
	{
		UpdateVersion_CollectClassData();
	}

	GraphVersion = EQSGraphVersion::Latest;
	Modify();
}

void USelectionQueryGraph::UpdateVersion_NestedNodes()
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USelectionQueryGraphNode_Option* OptionNode = Cast<USelectionQueryGraphNode_Option>(Nodes[Idx]);
		if (OptionNode)
		{
			USelectionQueryGraphNode* Node = OptionNode;
			while (Node)
			{
				USelectionQueryGraphNode* NextNode = NULL;
				for (int32 iPin = 0; iPin < Node->Pins.Num(); iPin++)
				{
					UEdGraphPin* TestPin = Node->Pins[iPin];
					if (TestPin && TestPin->Direction == EGPD_Output)
					{
						for (int32 iLink = 0; iLink < TestPin->LinkedTo.Num(); iLink++)
						{
							UEdGraphPin* LinkedTo = TestPin->LinkedTo[iLink];
							USelectionQueryGraphNode_Test* LinkedTest = LinkedTo ? Cast<USelectionQueryGraphNode_Test>(LinkedTo->GetOwningNode()) : NULL;
							if (LinkedTest)
							{
								LinkedTest->ParentNode = OptionNode;
								OptionNode->SubNodes.Add(LinkedTest);

								NextNode = LinkedTest;
								break;
							}
						}

						break;
					}
				}

				Node = NextNode;
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

		USelectionQueryGraphNode_Option* OptionNode = Cast<USelectionQueryGraphNode_Option>(Nodes[Idx]);
		if (OptionNode && OptionNode->Pins.IsValidIndex(1))
		{
			OptionNode->Pins[1]->MarkPendingKill();
			OptionNode->Pins.RemoveAt(1);
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
		USelQueryOption* OptionInstance = MyNode ? Cast<USelQueryOption>(MyNode->NodeInstance) : nullptr;
		if (OptionInstance && OptionInstance->Generator)
		{
			NodeInstances.Add(OptionInstance->Generator);
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
		USelQueryOption* OptionInstance = MyNode ? Cast<USelQueryOption>(MyNode->NodeInstance) : nullptr;
		if (OptionInstance && OptionInstance->Generator)
		{
			MyNode->ErrorMessage = FGraphNodeClassHelper::GetDeprecationMessage(OptionInstance->Generator->GetClass());
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

	TSet<USelQueryTest*> ExistingTests;
	TSet<USelQueryOption*> ExistingNodes;
	TArray<USelQueryOption*> OptionsCopy = QueryOwner->GetOptions();

	UAIGraphNode* MyRootNode = nullptr;
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USelectionQueryGraphNode* MyNode = Cast<USelectionQueryGraphNode>(Nodes[Idx]);
		USelQueryOption* OptionInstance = MyNode ? Cast<USelQueryOption>(MyNode->NodeInstance) : nullptr;
		if (OptionInstance && OptionInstance->Generator)
		{
			ExistingNodes.Add(OptionInstance);

			ExistingTests.Empty(ExistingTests.Num());
			for (int32 SubIdx = 0; SubIdx < MyNode->SubNodes.Num(); SubIdx++)
			{
				USelectionQueryGraphNode* MySubNode = Cast<USelectionQueryGraphNode>(MyNode->SubNodes[SubIdx]);
				USelQueryTest* TestInstance = MySubNode ? Cast<USelQueryTest>(MySubNode->NodeInstance) : nullptr;
				if (TestInstance)
				{
					ExistingTests.Add(TestInstance);
				}
				else
				{
					MyNode->RemoveSubNode(MySubNode);
					SubIdx--;
				}
			}

			SpawnMissingSubNodes(OptionInstance, ExistingTests, MyNode);
		}

		USelectionQueryGraphNode_Root* RootNode = Cast<USelectionQueryGraphNode_Root>(Nodes[Idx]);
		if (RootNode)
		{
			MyRootNode = RootNode;
		}
	}

	UEdGraphPin* RootOutPin = MyRootNode ? FindGraphNodePin(MyRootNode, EGPD_Output) : nullptr;
	ExistingTests.Empty(0);

	for (int32 Idx = 0; Idx < OptionsCopy.Num(); Idx++)
	{
		USelQueryOption* OptionInstance = OptionsCopy[Idx];
		if (ExistingNodes.Contains(OptionInstance) || OptionInstance == nullptr || OptionInstance->Generator == nullptr)
		{
			continue;
		}

		FGraphNodeCreator<USelectionQueryGraphNode_Option> NodeBuilder(*this);
		USelectionQueryGraphNode_Option* MyNode = NodeBuilder.CreateNode();
		UAIGraphNode::UpdateNodeClassDataFrom(OptionInstance->Generator->GetClass(), MyNode->ClassData);
		MyNode->ErrorMessage = MyNode->ClassData.GetDeprecatedMessage();
		NodeBuilder.Finalize();

		if (MyRootNode)
		{
			MyNode->NodePosX = MyRootNode->NodePosX + (Idx * 300);
			MyNode->NodePosY = MyRootNode->NodePosY + 100;
		}

		MyNode->NodeInstance = OptionInstance;
		SpawnMissingSubNodes(OptionInstance, ExistingTests, MyNode);

		UEdGraphPin* SpawnedInPin = FindGraphNodePin(MyNode, EGPD_Input);
		if (RootOutPin && SpawnedInPin)
		{
			RootOutPin->MakeLinkTo(SpawnedInPin);
		}
	}
}

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
}
