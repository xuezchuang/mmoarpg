// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleNumericalDeductionType.h"
#include "NumericalAlgorithmExecuteObject.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SIMPLENUMERICALDEDUCTIONRUNTIME_API UNumericalAlgorithmExecuteObject : public UObject
{
	GENERATED_BODY()

public:
	virtual float GetFloatAlgorithmValue(const FNAEParam& InParam);
	virtual int32 GetInt32AlgorithmValue(const FNAEParam& InParam);
	virtual FString GetStringAlgorithmValue(const FNAEParam& InParam);
public:
	virtual float GetDamageAlgorithmValue(const TMap<FName, float>& InLvActiveData, const TMap<FName, float>& InLvPassiveData);
	virtual float GetTreatmentAlgorithmValue(const TMap<FName, float>& InLvActiveData, const TMap<FName, float>& InLvPassiveData);
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "AlgorithmValue", meta = (DisplayName = "GetTreatmentAlgorithmValue"))
	float K2_GetTreatmentAlgorithmValue(const TMap<FName, float>& InLvActiveData, const TMap<FName, float>& InLvPassiveData);
	float K2_GetTreatmentAlgorithmValue_Implementation(const TMap<FName, float>& InLvActiveData, const TMap<FName, float>& InLvPassiveData) { return 0.f; };


	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "AlgorithmValue", meta = (DisplayName = "GetDamageAlgorithmValue"))
	float K2_GetDamageAlgorithmValue(const TMap<FName, float>& InLvActiveData, const TMap<FName, float>& InLvPassiveData);
	float K2_GetDamageAlgorithmValue_Implementation(const TMap<FName, float>& InLvActiveData, const TMap<FName, float>& InLvPassiveData) { return 0.f; };

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "AlgorithmValue", meta = (DisplayName = "GetFloatAlgorithmValue"))
	float K2_GetAlgorithmValue_Float(const FNAEParam& InParam);
	float K2_GetAlgorithmValue_Float_Implementation(const FNAEParam& InParam) { return 0.f; };

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "AlgorithmValue", meta = (DisplayName = "GetInt32AlgorithmValue"))
	int32 K2_GetAlgorithmValue_Int32(const FNAEParam& InParam);
	int32 K2_GetAlgorithmValue_Int32_Implementation(const FNAEParam& InParam) { return 0; };

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "AlgorithmValue", meta = (DisplayName = "GetStringAlgorithmValue"))
	FString K2_GetAlgorithmValue_String(const FNAEParam& InParam);
	FString K2_GetAlgorithmValue_String_Implementation(const FNAEParam& InParam) { return TEXT(""); };

};