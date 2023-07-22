// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Data2AnimSequenceStyle.h"

class FData2AnimSequenceCommands : public TCommands<FData2AnimSequenceCommands>
{
public:

	FData2AnimSequenceCommands()
		: TCommands<FData2AnimSequenceCommands>(TEXT("Data2AnimSequence"), NSLOCTEXT("Contexts", "Data2AnimSequence", "Data2AnimSequence Plugin"), NAME_None, FData2AnimSequenceStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
