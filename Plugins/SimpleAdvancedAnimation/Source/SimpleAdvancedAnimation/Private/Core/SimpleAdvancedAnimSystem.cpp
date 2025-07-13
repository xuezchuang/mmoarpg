// Copyright (C) RenZhai.2020.All Rights Reserved.
#include "Core/SimpleAdvancedAnimSystem.h"

FSimpleAdvancedAnimSystem * FSimpleAdvancedAnimSystem::AdvancedAnimSystem = NULL;

FSimpleAdvancedAnimSystem* FSimpleAdvancedAnimSystem::Get()
{
	if (!AdvancedAnimSystem)
	{
		AdvancedAnimSystem = new FSimpleAdvancedAnimSystem();
	}

	return AdvancedAnimSystem;
}

void FSimpleAdvancedAnimSystem::Destroy()
{
	if (AdvancedAnimSystem)
	{
		delete AdvancedAnimSystem;
		AdvancedAnimSystem = NULL;
	}
}

TStatId FSimpleAdvancedAnimSystem::GetStatId() const
{
	return TStatId();
}

void FSimpleAdvancedAnimSystem::Tick(float DeltaTime)
{
	FootIKSystem.Tick(DeltaTime);
	MantleCheckSystem.Tick(DeltaTime);
}