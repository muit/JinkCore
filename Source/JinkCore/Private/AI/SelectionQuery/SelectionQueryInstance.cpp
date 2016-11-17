// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "AI/SelectionQuery/SelectionQueryTypes.h"
#include "AI/SelectionQuery/SelectionQueryManager.h"


//----------------------------------------------------------------------//
// FSelectionQueryInstance
//----------------------------------------------------------------------//

void FSelectionQueryInstance::ExecuteOneStep()
{
    /*
	if (!Owner.IsValid())
	{
		MarkAsOwnerLost();
		return;
	}

	check(IsFinished() == false);

	if (!Options.IsValidIndex(OptionIndex))
	{
		NumValidItems = 0;
		FinalizeQuery();
		return;
	}

	SCOPE_CYCLE_COUNTER(STAT_AI_EQS_ExecuteOneStep);

	FSelectionQueryOptionInstance& OptionItem = Options[OptionIndex];
	const double StepStartTime = FPlatformTime::Seconds();

	const bool bDoingLastTest = (CurrentTest >= OptionItem.Tests.Num() - 1);
	bool bStepDone = true;
	CurrentStepTimeLimit = InCurrentStepTimeLimit;

	if (CurrentTest < 0)
	{
		SCOPE_CYCLE_COUNTER(STAT_AI_EQS_GeneratorTime);
		DEC_DWORD_STAT_BY(STAT_AI_EQS_NumItems, Items.Num());

		TotalExecutionTime = 0.0;
		GenerationExecutionTime = 0.0;
		PerStepExecutionTime.Empty(OptionItem.Tests.Num());
		PerStepExecutionTime.AddZeroed(OptionItem.Tests.Num());
		RawData.Reset();
		Items.Reset();
		ItemType = OptionItem.ItemType;
		bPassOnSingleResult = false;
		ValueSize = (ItemType->GetDefaultObject<USelectionQueryItemType>())->GetValueSize();
		
		{
			FScopeCycleCounterUObject GeneratorScope(OptionItem.Generator);
			OptionItem.Generator->GenerateItems(*this);
		}

		FinalizeGeneration();

		GenerationExecutionTime = FPlatformTime::Seconds() - StepStartTime;
		TotalExecutionTime += GenerationExecutionTime;
	}
	else if (OptionItem.Tests.IsValidIndex(CurrentTest))
	{
		SCOPE_CYCLE_COUNTER(STAT_AI_EQS_TestTime);

		USelectionQueryTest* TestObject = OptionItem.Tests[CurrentTest];

		// item generator uses this flag to alter the scoring behavior
		bPassOnSingleResult = (bDoingLastTest && Mode == ESelectionQueryRunMode::SingleResult && TestObject->CanRunAsFinalCondition());

		if (bPassOnSingleResult)
		{
			// Since we know we're the last test that is a final condition, if we were scoring previously we should sort the tests now before we test them
			bool bSortTests = false;
			for (int32 TestIndex = 0; TestIndex < OptionItem.Tests.Num() - 1; ++TestIndex)
			{
				if (OptionItem.Tests[TestIndex]->TestPurpose != EEnvTestPurpose::Filter)
				{
					// Found one.  We should sort.
					bSortTests = true;
					break;
				}
			}

			if (bSortTests)
			{
				SortScores();
			}
		}

		const int32 ItemsAlreadyProcessed = CurrentTestStartingItem;

		{
			FScopeCycleCounterUObject TestScope(TestObject);
			TestObject->RunTest(*this);
		}

		bStepDone = CurrentTestStartingItem >= Items.Num() || bFoundSingleResult
			// or no items processed ==> this means error
			|| (ItemsAlreadyProcessed == CurrentTestStartingItem);

		if (bStepDone)
		{
			FinalizeTest();
		}

		const double StepTime = (FPlatformTime::Seconds() - StepStartTime);
		PerStepExecutionTime[CurrentTest] += StepTime;
		TotalExecutionTime += StepTime;
	}
	else
	{
		UE_LOG(LogSelectionQuery, Warning, TEXT("Query [%s] is trying to execute non existing test! [option:%d test:%d]"), 
			*QueryName, OptionIndex, CurrentTest);
	}
	
	if (bStepDone)
	{
#if USE_EQS_DEBUGGER
		if (bStoreDebugInfo)
		{
			DebugData.Store(this);
		}
#endif // USE_EQS_DEBUGGER

		CurrentTest++;
		CurrentTestStartingItem = 0;
	}

	// sort results or switch to next option when all tests are performed
	if (IsFinished() == false &&
		(OptionItem.Tests.Num() == CurrentTest || NumValidItems <= 0))
	{
		if (NumValidItems > 0)
		{
			// found items, sort and finish
			FinalizeQuery();
		}
		else
		{
			// no items here, go to next option or finish			
			if (OptionIndex + 1 >= Options.Num())
			{
				// out of options, finish processing without errors
				FinalizeQuery();
			}
			else
			{
				// not doing it always for debugging purposes
				OptionIndex++;
				CurrentTest = -1;
			}
		}
	}*/
}

#if !NO_LOGGING
void FSelectionQueryInstance::Log(const FString Msg) const
{
	UE_LOG(LogSelectionQuery, Warning, TEXT("%s"), *Msg);
}
#endif // !NO_LOGGING

void FSelectionQueryInstance::NormalizeScores()
{
    /*
	// @note this function assumes results have been already sorted and all first NumValidItems
	// items in Items are valid (and the rest is not).
	check(NumValidItems <= Items.Num())

	float MinScore = 0.f;
	float MaxScore = -BIG_NUMBER;

	FSelectionQueryItem* ItemInfo = Items.GetData();
	for (int32 ItemIndex = 0; ItemIndex < NumValidItems; ItemIndex++, ItemInfo++)
	{
		ensure(ItemInfo->IsValid());

		MinScore = FMath::Min(MinScore, ItemInfo->Score);
		MaxScore = FMath::Max(MaxScore, ItemInfo->Score);
	}

	ItemInfo = Items.GetData();
	if (MinScore == MaxScore)
	{
		const float Score = (MinScore == 0.f) ? 0.f : 1.f;
		for (int32 ItemIndex = 0; ItemIndex < NumValidItems; ItemIndex++, ItemInfo++)
		{
			ItemInfo->Score = Score;
		}
	}
	else
	{
		const float ScoreRange = MaxScore - MinScore;
		for (int32 ItemIndex = 0; ItemIndex < NumValidItems; ItemIndex++, ItemInfo++)
		{
			ItemInfo->Score = (ItemInfo->Score - MinScore) / ScoreRange;
		}
	}*/
}

void FSelectionQueryInstance::SortScores()
{
}

void FSelectionQueryInstance::PickSingleItem(int32 ItemIndex)
{
    /*
	check(Items.Num() > 0);

	if (Items.IsValidIndex(ItemIndex) == false)
	{
		UE_LOG(LogSelectionQuery, Warning
			, TEXT("Query [%s] tried to pick item %d as best item, but this index is out of scope (num items: %d). Falling back to item 0.")
			, *QueryName, ItemIndex, Items.Num());
		ItemIndex = 0;
	}

	FSelectionQueryItem BestItem;
	// Copy the score from the actual item rather than just putting "1".  That way, it will correctly show cases where
	// the final filtering test was skipped by an item (and therefore not failed, i.e. passed).
	BestItem.Score = Items[ItemIndex].Score;
	BestItem.DataOffset = Items[ItemIndex].DataOffset;

	Items.Empty(1);
	Items.Add(BestItem);*/
}

void FSelectionQueryInstance::FinalizeQuery()
{
	if (Mode == ESQRunMode::SingleResult)
	{
		//Select the item
        PickSingleItem(0);
	}
	else
	{
		SortScores();

		// normalizing after scoring and reducing number of elements to not 
		// do anything for discarded items
		NormalizeScores();
	}

	MarkAsFinishedWithoutIssues();
}
