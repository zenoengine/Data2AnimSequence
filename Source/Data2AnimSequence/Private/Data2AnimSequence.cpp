// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data2AnimSequence.h"
#include "Data2AnimSequenceStyle.h"
#include "Data2AnimSequenceCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#if WITH_EDITOR
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"
#endif

static const FName Data2AnimSequenceTabName("Data2AnimSequence");

#define LOCTEXT_NAMESPACE "FData2AnimSequenceModule"

void FData2AnimSequenceModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FData2AnimSequenceStyle::Initialize();
	FData2AnimSequenceStyle::ReloadTextures();

	FData2AnimSequenceCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FData2AnimSequenceCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FData2AnimSequenceModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FData2AnimSequenceModule::RegisterMenus));
}

void FData2AnimSequenceModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FData2AnimSequenceStyle::Shutdown();

	FData2AnimSequenceCommands::Unregister();
}

void FData2AnimSequenceModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
		LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
		FText::FromString(TEXT("FAnimSequenceGenModule::PluginButtonClicked()")),
		FText::FromString(TEXT("AnimSequenceGen.cpp"))
	);

	// create UAnimSequence asset
	FString AnimSequenceName = TEXT("Duumy");
	UPackage* AnimSequencePackage = Cast<UPackage>(GetTransientPackage());

#if WITH_EDITOR
	// we have to check InParent is not valid as this will tell us whether we import asset via editor asset factory or not
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

	FString DefaultSuffix;
	FString AssetName;
	//FString PackageName;
	//const FString BasePackageName(FString::Printf(TEXT("/AnimSequenceGen/Assets/AnimSequence"), *AnimSequenceName));
	//AssetToolsModule.Get().CreateUniqueAssetName(BasePackageName, DefaultSuffix, /*out*/ PackageName, /*out*/ AssetName);

	// Create a unique package name and asset name
	FString PackageName = TEXT("/Game/") + AnimSequenceName;
	AnimSequencePackage = CreatePackage(*PackageName);
	AnimSequencePackage->SetFlags(RF_Standalone | RF_Public);
	AnimSequencePackage->FullyLoad();
	AnimSequencePackage->MarkPackageDirty();

#endif

	// AssetRegistryModule을 사용하여 Asset을 검색합니다.
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetData;
	FARFilter Filter;
	FString AssetPath = "/Game/Characters/Mannequins/Meshes/";
	Filter.PackagePaths.Add(*AssetPath);
	AssetRegistry.GetAssets(Filter, AssetData);

	if (AssetData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skeleton Asset not found in path: %s"), *AssetPath);
		return;
	}

	// 첫 번째로 검색된 Skeleton Asset을 로드합니다.
	FString AssetReference = AssetData[0].GetExportTextName();
	USkeleton* LoadedSkeleton = Cast<USkeleton>(StaticLoadObject(USkeleton::StaticClass(), nullptr, *AssetReference));


	UAnimSequence* AnimSequence = NewObject<UAnimSequence>(IsValid(AnimSequencePackage) ? AnimSequencePackage : GetTransientPackage(), *AnimSequenceName, RF_Public | RF_Standalone);
	{
		AnimSequence->SetSkeleton(LoadedSkeleton);
		// AnimSequence->SetSequenceLength(SequenceLength);
	}

#if WITH_EDITOR
	// Create in-editor data   
	IAnimationDataController& AnimController = AnimSequence->GetController();

	AnimController.SetModel(AnimSequence->GetDataModelInterface());

	AnimController.OpenBracket(LOCTEXT("MetahumanSDKAnimSequenceFactory", "Adding float curve."), false);
	{
		const float FPS = 60;
		const int32 NumFrames = 60 * 2;
		AnimController.InitializeModel();
		AnimController.SetFrameRate(FFrameRate(FPS, 1.f));
		AnimController.SetNumberOfFrames(NumFrames);

		//for (auto& Track : BoneTracks)
		//{
		//	AnimController.AddBoneCurve(Track.Key);
		//	AnimController.SetBoneTrackKeys(Track.Key, Track.Value.PosKeys, Track.Value.RotKeys, Track.Value.ScaleKeys);
		//}

		FName BoneName = TEXT("Pelvis");
		AnimController.AddBoneCurve(BoneName);

		TArray<FVector3f> PositionalKeys;
		TArray<FQuat4f> RotationalKeys;
		TArray<FVector3f> ScalingKeys;

		for (int32 Frame = 0; Frame < NumFrames; Frame++)
		{
			PositionalKeys.Add(FVector3f(Frame * 1, 0, 0));
			ScalingKeys.Add(FVector3f(1, 1, 1));
			FQuat4f Qat = FQuat4f(FRotator(0, 0, Frame * 1).Quaternion());
			RotationalKeys.Add({});
		}

		AnimController.SetBoneTrackKeys(BoneName, PositionalKeys, RotationalKeys, ScalingKeys);

		AnimController.NotifyPopulated();
	}
	AnimController.CloseBracket(false);

	FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	FString PackageFileName = FPackageName::LongPackageNameToFilename(
		PackageName, FPackageName::GetAssetPackageExtension());

	bool bSaved = UPackage::SavePackage(
		AnimSequencePackage,
		AnimSequence,
		EObjectFlags::RF_Public | EObjectFlags::RF_Standalone,
		*PackageFileName,
		GError, nullptr, true, true, SAVE_NoError);

	TArray<UObject*> ObjectsToSync;
	ObjectsToSync.Add(AnimSequence);
	GEditor->SyncBrowserToObjects(ObjectsToSync);
#endif
}

void FData2AnimSequenceModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FData2AnimSequenceCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FData2AnimSequenceCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FData2AnimSequenceModule, Data2AnimSequence)