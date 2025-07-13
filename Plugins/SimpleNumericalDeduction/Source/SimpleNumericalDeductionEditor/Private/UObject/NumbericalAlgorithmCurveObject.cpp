// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "UObject/NumbericalAlgorithmCurveObject.h"

TArray<FRichCurveEditInfo> UNumbericalAlgorithmCurveObject::GetCurves()
{
	return Curves;
}

void UNumbericalAlgorithmCurveObject::AddCurves(const FRichCurveEditInfo& InCurve)
{
	Curves.Add(InCurve);
}