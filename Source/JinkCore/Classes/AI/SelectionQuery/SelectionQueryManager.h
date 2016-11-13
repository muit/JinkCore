// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "SelectionQueryManager.generated.h"

class USelectionQueryComponent;
class USQCompositeNode;
class USQDecorator;
class USelectionQuery;

USTRUCT()
struct FSelectionQueryTemplateInfo
{
	GENERATED_USTRUCT_BODY()

	/** behavior tree asset */
	UPROPERTY()
	USelectionQuery* Asset;

	/** initialized template */
	UPROPERTY(transient)
	USQCompositeNode* Template;

	/** size required for instance memory */
	uint16 InstanceMemorySize;
};

UCLASS(config=Engine)
class AIMODULE_API USelectionQueryManager : public UObject
{
	GENERATED_UCLASS_BODY()

	/** limit for recording execution steps for debugger */
	UPROPERTY(config)
	int32 MaxDebuggerSteps;

	/** get behavior tree template for given blueprint */
	bool LoadTree(USelectionQuery& Asset, USQCompositeNode*& Root, uint16& InstanceMemorySize);

	/** get aligned memory size */
	static int32 GetAlignedDataSize(int32 Size);

	/** helper function for sorting and aligning node memory */
	static void InitializeMemoryHelper(const TArray<USQDecorator*>& Nodes, TArray<uint16>& MemoryOffsets, int32& MemorySize, bool bForceInstancing = false);

	/** cleanup hooks for map loading */
	virtual void FinishDestroy() override;

	void DumpUsageStats() const;

	/** register new behavior tree component for tracking */
	void AddActiveComponent(USelectionQueryComponent& Component);

	/** unregister behavior tree component from tracking */
	void RemoveActiveComponent(USelectionQueryComponent& Component);

	static USelectionQueryManager* GetCurrent(UWorld* World);
	static USelectionQueryManager* GetCurrent(UObject* WorldContextObject);

protected:

	/** initialized tree templates */
	UPROPERTY()
	TArray<FSelectionQueryTemplateInfo> LoadedTemplates;

	UPROPERTY()
	TArray<USelectionQueryComponent*> ActiveComponents;
};
