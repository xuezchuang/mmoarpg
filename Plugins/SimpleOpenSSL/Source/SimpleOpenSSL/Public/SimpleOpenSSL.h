// Copyright (C) RenZhai.2022.All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FSimpleOpenSSLModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
