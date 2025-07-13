// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "SimpleNumericalDeductionCommands.h"

#define LOCTEXT_NAMESPACE "FSimpleNumericalDeductionModule"

void FSimpleNumericalDeductionCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "SimpleNumericalDeduction", "Bring up SimpleNumericalDeduction window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
