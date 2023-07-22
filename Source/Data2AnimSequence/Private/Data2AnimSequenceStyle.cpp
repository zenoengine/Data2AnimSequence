// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data2AnimSequenceStyle.h"
#include "Data2AnimSequence.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FData2AnimSequenceStyle::StyleInstance = nullptr;

void FData2AnimSequenceStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FData2AnimSequenceStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FData2AnimSequenceStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("Data2AnimSequenceStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FData2AnimSequenceStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("Data2AnimSequenceStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("Data2AnimSequence")->GetBaseDir() / TEXT("Resources"));

	Style->Set("Data2AnimSequence.PluginAction", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	return Style;
}

void FData2AnimSequenceStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FData2AnimSequenceStyle::Get()
{
	return *StyleInstance;
}
