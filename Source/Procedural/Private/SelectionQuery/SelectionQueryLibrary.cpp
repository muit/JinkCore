// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "ProceduralPrivatePCH.h"
#include "SelectionQueryLibrary.h"

void USelectionQueryLibrary::RunQuery(TSubclassOf<USelectionQuery> Query, bool& Success, TSubclassOf<UItem>& Item)
{
    //Create the Query
    USelectionQuery* QueryObj = NewObject<USelectionQuery>((UObject*)GetTransientPackage(), Query);

    FSQExecution Result = QueryObj->Execute();
    
    Success = Result.IsValid;
    Item =  Result.ItemValue;
}

void USelectionQueryLibrary::GetAllQueryItems(TSubclassOf<USelectionQuery> Query, bool& Success, TArray<TSubclassOf<UItem>>& Items)
{
    //Create the Query
    USelectionQuery* QueryObj = NewObject<USelectionQuery>((UObject*)GetTransientPackage(), Query);

    FSQExecution Result = QueryObj->Execute();

    Success = Result.IsValid;
    Items = Result.ItemRecord.Array();
}

UClass* USelectionQueryLibrary::RunClassPT(FPTClassExecution& Node)
{
    return Node.ItemValue;
}

/*TAssetPtr<UObject>& USelectionQueryLibrary::RunAssetPT()
{
    return TAssetPtr<UObject>();
}*/

TAssetSubclassOf<UDataAsset> USelectionQueryLibrary::RunDataAssetPT(FPTDataAssetExecution& Node)
{
    return Node.ItemValue;
}
