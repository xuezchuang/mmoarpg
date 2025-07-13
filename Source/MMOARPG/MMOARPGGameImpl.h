// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClientDataStoe.h"

class MMOARPGGameImpl : public FDefaultGameModuleImpl
{
public:

	MMOARPGGameImpl();

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FClientDataStore& GetClientDataStore();
private:
	static FClientDataStore ClientDataStore;

};