// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "SimplePopup.h"

#define LOCTEXT_NAMESPACE "FSimplePopupModule"

void FSimplePopupModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FSimplePopupModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimplePopupModule, SimplePopup)