// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widget/Editor/Core/DeduceEditor.h"
#include "SimpleNumericalDeductionType.h"

class FDebugAttibuteDeduce :public FDeduceEditor
{
public:
	FDebugAttibuteDeduce();
	virtual ~FDebugAttibuteDeduce(){ }
	virtual void InitLayout();

public:
	void Construct();

	TSharedRef<SDockTab> SpawnTab_DebugSettingWidget(const FSpawnTabArgs& Args);
private:

	TSharedPtr<SWidget> WidgetBase;
};