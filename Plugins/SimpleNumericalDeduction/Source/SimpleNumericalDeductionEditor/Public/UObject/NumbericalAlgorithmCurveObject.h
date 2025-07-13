// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleNumericalDeductionType.h"
#include "Curves/CurveBase.h"
#include "NumbericalAlgorithmCurveObject.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SIMPLENUMERICALDEDUCTION_API UNumbericalAlgorithmCurveObject : public UCurveBase
{
	GENERATED_BODY()

public:
	virtual TArray<FRichCurveEditInfo> GetCurves() override;
	void AddCurves(const FRichCurveEditInfo& InCurve);
protected:
	TArray<FRichCurveEditInfo> Curves;
};