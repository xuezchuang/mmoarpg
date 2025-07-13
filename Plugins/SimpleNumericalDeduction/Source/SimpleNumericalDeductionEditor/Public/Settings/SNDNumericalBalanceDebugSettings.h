// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleNumericalDeductionType.h"
#include "SNDNumericalBalanceDebugSettings.generated.h"

UCLASS(config = SNDNumericalBalanceDebugSettings)
class USNDNumericalBalanceDebugSettings : public UObject
{
	GENERATED_BODY()

public:
	USNDNumericalBalanceDebugSettings();

	UPROPERTY(EditAnywhere)
	bool bIterationLevel;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIterationLevel"))
	EIterativeDebugPrintMethod IterativeMethod;

	UPROPERTY(EditAnywhere, Category = NumericalDeductionNumericalBalanceDebug, meta = (ToolTip = "This content is version base, which is used for automatic iteration."))
	TArray<FDebugCharactersInfo> DebugCharactersInfo;
};