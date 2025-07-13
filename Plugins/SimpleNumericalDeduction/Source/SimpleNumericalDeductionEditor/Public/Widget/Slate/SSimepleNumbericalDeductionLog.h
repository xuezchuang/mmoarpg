// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleNumericalDeductionType.h"
#include "Widgets/SCompoundWidget.h"
#include "Tickable.h"

class SVerticalBox;
class SSimepleNumbericalDeductionLog :public SCompoundWidget, public FTickableGameObject
{
public:
	SLATE_BEGIN_ARGS(SSimepleNumbericalDeductionLog)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

public:
	virtual bool IsTickableInEditor() const;
	virtual void Tick(float DeltaTime);
	virtual TStatId GetStatId() const;
public:
	void Generate();
	void AddLog(const FSimplePreDebugPrintf& MyLog);
	void ClearLog();
	void SaveAsText();
public:
	TSharedPtr<SVerticalBox> BoxList;
};