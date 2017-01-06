// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkEditorPrivatePCH.h"
#include "ContentBrowserModule.h"
#include "LevelInstance/LevelInstance.h"
#include "Asset/LevelInstanceFactory.h"
#include "ContentBrowserExtensions.h"

#define LOCTEXT_NAMESPACE "JinkCore"

//////////////////////////////////////////////////////////////////////////

FContentBrowserMenuExtender_SelectedAssets ContentBrowserExtenderDelegate;
FDelegateHandle ContentBrowserExtenderDelegateHandle;

//////////////////////////////////////////////////////////////////////////
// FContentBrowserSelectedAssetExtensionBase

struct FContentBrowserSelectedAssetExtensionBase
{
public:
    TArray<class FAssetData> SelectedAssets;

public:
    virtual void Execute() {}
    virtual ~FContentBrowserSelectedAssetExtensionBase() {}
};

//////////////////////////////////////////////////////////////////////////
// FCreateLevelInstanceFromLevelExtension

#include "AssetToolsModule.h"
#include "AssetRegistryModule.h"


struct FCreateLevelInstanceFromLevelExtension : public FContentBrowserSelectedAssetExtensionBase
{
    bool bExtractLevels;

    FCreateLevelInstanceFromLevelExtension()
        : bExtractLevels(false)
    {
    }

    void CreateLevelInstanceFromLevel(TArray<TAssetPtr<UWorld>>& Levels)
    {
        FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
        FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

        TArray<UObject*> ObjectsToSync;

        for (auto LevelIt = Levels.CreateConstIterator(); LevelIt; ++LevelIt)
        {
            TAssetPtr<UWorld> Level = *LevelIt;

            // Create the factory used to generate the sprite
            ULevelInstanceFactory* LevelInstanceFactory = NewObject<ULevelInstanceFactory>();
            LevelInstanceFactory->InitialLevel = Level;

            // Create the sprite
            FString Name;
            FString PackageName;

            if (!bExtractLevels)
            {
                // Get a unique name for the level instance
                const FString DefaultSuffix = TEXT("_Instance");
                AssetToolsModule.Get().CreateUniqueAssetName(Level->GetOutermost()->GetName(), DefaultSuffix, /*out*/ PackageName, /*out*/ Name);
                const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);

                if (UObject* NewAsset = AssetToolsModule.Get().CreateAsset(Name, PackagePath, ULevelInstance::StaticClass(), LevelInstanceFactory))
                {
                    ObjectsToSync.Add(NewAsset);
                }
            }
            else
            {
                // Window Dialog
            }
        }

        if (ObjectsToSync.Num() > 0)
        {
            ContentBrowserModule.Get().SyncBrowserToAssets(ObjectsToSync);
        }
    }

    virtual void Execute() override
    {
        // Create sprites for any selected textures
        TArray<TAssetPtr<UWorld>> Levels;
        for (auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt)
        {
            const FAssetData& AssetData = *AssetIt;
            if (TAssetPtr<UWorld> Level = Cast<UWorld>(AssetData.GetAsset()))
            {
                Levels.Add(Level);
            }
        }

        CreateLevelInstanceFromLevel(Levels);
    }
};

//////////////////////////////////////////////////////////////////////////
// FJCContentBrowserExtensions_Impl

class FJCContentBrowserExtensions_Impl
{
public:
    static void ExecuteSelectedContentFunctor(TSharedPtr<FContentBrowserSelectedAssetExtensionBase> SelectedAssetFunctor)
    {
        SelectedAssetFunctor->Execute();
    }

    static void CreateLevelActionsSubMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets)
    {
        // Create level instances
        TSharedPtr<FCreateLevelInstanceFromLevelExtension> LevelICreatorFunctor = MakeShareable(new FCreateLevelInstanceFromLevelExtension());
        LevelICreatorFunctor->SelectedAssets = SelectedAssets;

        FUIAction Action_CreateLevelInstancesFromLevels(
            FExecuteAction::CreateStatic(&FJCContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor, StaticCastSharedPtr<FContentBrowserSelectedAssetExtensionBase>(LevelICreatorFunctor)));

        const FName JCStyleSetName = FEditorStyle::Get().GetStyleSetName();

        MenuBuilder.AddMenuEntry(
            LOCTEXT("CreateLevelInstance", "Create Level Instance"),
            LOCTEXT("CreateLevelInstance_Tooltip", "Create level instances from selected levels"),
            FSlateIcon(FEditorStyle::GetStyleSetName(), "ClassIcon.LevelBounds"),
            Action_CreateLevelInstancesFromLevels,
            NAME_None,
            EUserInterfaceActionType::Button);

        /*
        MenuBuilder.AddSubMenu(
            LOCTEXT("LevelActionsSubMenuLabel", "Level Actions"),
            LOCTEXT("LevelActionsSubMenuToolTip", "Level-related actions for this texture."),
            FNewMenuDelegate::CreateStatic(&FJCContentBrowserExtensions_Impl::PopulateLevelActionsMenu, SelectedAssets),
            false,
            FSlateIcon(FEditorStyle::GetStyleSetName(), "ClassIcon.LevelBounds")
        );*/
    }

    static void PopulateLevelActionsMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets)
    {
        //Other Extensions
        //
    }

    static TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets)
    {
        // Run through the assets to determine if any meet our criteria
        bool bAnyLevel = false;
        for (auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt)
        {
            bAnyLevel = bAnyLevel || (AssetIt->AssetClass == UWorld::StaticClass()->GetFName());
        }

        TSharedRef<FExtender> Extender(new FExtender());

        if (bAnyLevel)
        {
            // Add the sprite actions sub-menu extender
            Extender->AddMenuExtension(
                "CommonAssetActions",
                EExtensionHook::After,
                nullptr,
                FMenuExtensionDelegate::CreateStatic(&FJCContentBrowserExtensions_Impl::CreateLevelActionsSubMenu, SelectedAssets));
        }

        return Extender;
    }

    static TArray<FContentBrowserMenuExtender_SelectedAssets>& GetExtenderDelegates()
    {
        FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
        return ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
    }
};

//////////////////////////////////////////////////////////////////////////
// FJCContentBrowserExtensions

void FJCContentBrowserExtensions::InstallHooks()
{
    ContentBrowserExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&FJCContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu);

    TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuExtenderDelegates = FJCContentBrowserExtensions_Impl::GetExtenderDelegates();
    CBMenuExtenderDelegates.Add(ContentBrowserExtenderDelegate);
    ContentBrowserExtenderDelegateHandle = CBMenuExtenderDelegates.Last().GetHandle();
}

void FJCContentBrowserExtensions::RemoveHooks()
{
    TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuExtenderDelegates = FJCContentBrowserExtensions_Impl::GetExtenderDelegates();
    CBMenuExtenderDelegates.RemoveAll([](const FContentBrowserMenuExtender_SelectedAssets& Delegate) { return Delegate.GetHandle() == ContentBrowserExtenderDelegateHandle; });
}

//////////////////////////////////////////////////////////////////////////
