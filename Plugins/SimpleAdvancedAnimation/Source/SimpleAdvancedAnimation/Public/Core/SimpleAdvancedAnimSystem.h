// Copyright (C) RenZhai.2020.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "Core/Element/FootIK/SimpleFootIKSystem.h"
#include "Core/Element/MantleCheck/SimpleMantleCheckSystem.h"
#include "SimpleAdvancedAnimationType.h"

class FSimpleAdvancedAnimSystem: public FTickableGameObject
{
public:
	virtual void Tick(float DeltaTime);
	virtual TStatId GetStatId() const;

	static FSimpleAdvancedAnimSystem *Get();
	static void Destroy();

public:
	FFootIKSystem *GetFootIKSystem() { return &FootIKSystem; }
protected:
	static FSimpleAdvancedAnimSystem* AdvancedAnimSystem;

	FFootIKSystem FootIKSystem;
	FSimpleMantleCheckSystem MantleCheckSystem;
};