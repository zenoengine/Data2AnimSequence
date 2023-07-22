// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data2AnimSequenceCommands.h"

#define LOCTEXT_NAMESPACE "FData2AnimSequenceModule"

void FData2AnimSequenceCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Data2AnimSequence", "Execute Data2AnimSequence action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
