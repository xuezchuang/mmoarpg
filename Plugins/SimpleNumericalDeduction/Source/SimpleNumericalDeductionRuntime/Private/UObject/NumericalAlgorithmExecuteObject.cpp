// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "UObject/NumericalAlgorithmExecuteObject.h"
#include "..\..\Public\UObject\NumericalAlgorithmExecuteObject.h"

float UNumericalAlgorithmExecuteObject::GetFloatAlgorithmValue(const FNAEParam& InParam)
{
	return K2_GetAlgorithmValue_Float(InParam);
}

int32 UNumericalAlgorithmExecuteObject::GetInt32AlgorithmValue(const FNAEParam& InParam)
{
	return K2_GetAlgorithmValue_Int32(InParam);
}

FString UNumericalAlgorithmExecuteObject::GetStringAlgorithmValue(const FNAEParam& InParam)
{
	return K2_GetAlgorithmValue_String(InParam);
}

float UNumericalAlgorithmExecuteObject::GetDamageAlgorithmValue(const TMap<FName, float>& InLvActiveData, const TMap<FName, float>& InLvPassiveData)
{
	return K2_GetDamageAlgorithmValue(InLvActiveData, InLvPassiveData);
}

float UNumericalAlgorithmExecuteObject::GetTreatmentAlgorithmValue(const TMap<FName, float>& InLvActiveData, const TMap<FName, float>& InLvPassiveData)
{
	return K2_GetTreatmentAlgorithmValue(InLvActiveData, InLvPassiveData);
}
