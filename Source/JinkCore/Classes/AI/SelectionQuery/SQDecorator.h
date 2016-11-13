// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "SelectionQueryTypes.h"
#include "SelectionQuery/SQAuxiliaryNode.h"
#include "SQDecorator.generated.h"

class USQNode;
class USelectionQueryComponent;
class FBehaviorDecoratorDetails; 
struct FSelectionQuerySearchData;

/** 
 * Decorators are supporting nodes placed on parent-child connection, that receive notification about execution flow and can be ticked
 *
 * Because some of them can be instanced for specific AI, following virtual functions are not marked as const:
 *  - OnNodeActivation
 *  - OnNodeDeactivation
 *  - OnNodeProcessed
 *  - OnBecomeRelevant (from USQAuxiliaryNode)
 *  - OnCeaseRelevant (from USQAuxiliaryNode)
 *  - TickNode (from USQAuxiliaryNode)
 *
 * If your node is not being instanced (default behavior), DO NOT change any properties of object within those functions!
 * Template nodes are shared across all behavior tree components using the same tree asset and must store
 * their runtime properties in provided NodeMemory block (allocation size determined by GetInstanceMemorySize() )
 *
 */

UCLASS(Abstract)
class AIMODULE_API USQDecorator : public USQAuxiliaryNode
{
	GENERATED_UCLASS_BODY()

	/** wrapper for node instancing: CalculateRawConditionValue */
	bool WrappedCanExecute(USelectionQueryComponent& OwnerComp, uint8* NodeMemory) const;

	/** wrapper for node instancing: OnNodeActivation  */
	void WrappedOnNodeActivation(FSelectionQuerySearchData& SearchData) const;
	
	/** wrapper for node instancing: OnNodeDeactivation */
	void WrappedOnNodeDeactivation(FSelectionQuerySearchData& SearchData, ESQNodeResult::Type NodeResult) const;

	/** wrapper for node instancing: OnNodeProcessed */
	void WrappedOnNodeProcessed(FSelectionQuerySearchData& SearchData, ESQNodeResult::Type& NodeResult) const;

	/** @return flow controller's abort mode */
	ESQFlowAbortMode::Type GetFlowAbortMode() const;

	/** @return true if condition should be inversed */
	bool IsInversed() const;

	virtual FString GetStaticDescription() const override;

	/** modify current flow abort mode, so it can be used with parent composite */
	void UpdateFlowAbortMode();

	/** @return true if current abort mode can be used with parent composite */
	bool IsFlowAbortModeValid() const;

protected:

	/** if set, FlowAbortMode can be set to None */
	uint32 bAllowAbortNone : 1;

	/** if set, FlowAbortMode can be set to LowerPriority and Both */
	uint32 bAllowAbortLowerPri : 1;

	/** if set, FlowAbortMode can be set to Self and Both */
	uint32 bAllowAbortChildNodes : 1;

	/** if set, OnNodeActivation will be used */
	uint32 bNotifyActivation : 1;

	/** if set, OnNodeDeactivation will be used */
	uint32 bNotifyDeactivation : 1;

	/** if set, OnNodeProcessed will be used */
	uint32 bNotifyProcessed : 1;

	/** if set, static description will include default description of inversed condition */
	uint32 bShowInverseConditionDesc : 1;

private:
	/** if set, condition check result will be inversed */
	UPROPERTY(Category = Condition, EditAnywhere)
	uint32 bInverseCondition : 1;

protected:
	/** flow controller settings */
	UPROPERTY(Category=FlowControl, EditAnywhere)
	TEnumAsByte<ESQFlowAbortMode::Type> FlowAbortMode;

	void SetIsInversed(bool bShouldBeInversed);

	/** called when underlying node is activated
	  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void OnNodeActivation(FSelectionQuerySearchData& SearchData);

	/** called when underlying node has finished
	 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void OnNodeDeactivation(FSelectionQuerySearchData& SearchData, ESQNodeResult::Type NodeResult);

	/** called when underlying node was processed (deactivated or failed to activate)
	 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void OnNodeProcessed(FSelectionQuerySearchData& SearchData, ESQNodeResult::Type& NodeResult);

	/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
	virtual bool CalculateRawConditionValue(USelectionQueryComponent& OwnerComp, uint8* NodeMemory) const;

	friend FBehaviorDecoratorDetails;

	//----------------------------------------------------------------------//
	// DEPRECATED
	//----------------------------------------------------------------------//
	DEPRECATED(4.7, "This version is deprecated. Please use the one taking reference to USelectionQueryComponent rather than a pointer.")
	virtual bool CalculateRawConditionValue(USelectionQueryComponent* OwnerComp, uint8* NodeMemory) const;
	DEPRECATED(4.12, "This function is deprecated, please use InitializeParentLink instead.")
	void InitializeDecorator(uint8 InChildIndex);
};


//////////////////////////////////////////////////////////////////////////
// Inlines

FORCEINLINE ESQFlowAbortMode::Type USQDecorator::GetFlowAbortMode() const
{
	return FlowAbortMode;
}

FORCEINLINE bool USQDecorator::IsInversed() const
{
	return bInverseCondition;
}
