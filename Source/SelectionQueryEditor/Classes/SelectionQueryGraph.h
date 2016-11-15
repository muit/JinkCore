// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "AIGraph.h"
#include "SelectionQueryGraph.generated.h"

class USQComposerNode;
class USelectionQueryGraphNode;

UCLASS()
class USelectionQueryGraph : public UAIGraph
{
	GENERATED_UCLASS_BODY()

	virtual void Initialize() override;
	virtual void OnLoaded() override;
	virtual void UpdateVersion() override;
	virtual void MarkVersion() override;
	virtual void UpdateAsset(int32 UpdateFlags = 0) override;

	void UpdateDeprecatedGeneratorClasses();
	void SpawnMissingNodes();
	void CalculateAllWeights();
	void CreateSelQueryFromGraph(class USelectionQueryGraphNode* RootEdNode);

protected:

	void UpdateVersion_NestedNodes();
	void UpdateVersion_FixupOuters();
	void UpdateVersion_CollectClassData();

	virtual void CollectAllNodeInstances(TSet<UObject*>& NodeInstances) override;
	virtual void OnNodeInstanceRemoved(UObject* NodeInstance) override;
	virtual void OnNodesPasted(const FString& ImportStr) override;

	//void SpawnMissingSubNodes(USQCompositeNode* Composite, TSet<USelQueryTest*> ExistingTests, USelectionQueryGraphNode* CompositeNode);
};
