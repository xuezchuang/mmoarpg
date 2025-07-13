// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleNumericalDeductionType.generated.h"

class UNumericalAlgorithmExecuteObject;

UENUM(BlueprintType)
enum class EIterativeDebugPrintMethod :uint8
{
	ONE_TO_MANY_METHOD		UMETA(DisplayName = "1->N"),
	MANY_TO_MANY_METHOD		UMETA(DisplayName = "N->N"),
	MANY_TO_ONCE_METHOD		UMETA(DisplayName = "N->1"),
};

struct SIMPLENUMERICALDEDUCTIONRUNTIME_API FSimplePreDebugPrintf
{
	FString CharacterNameActive;
	FString CharacterNamePassive;
	FString EventString;
	FString Value;
};

struct SIMPLENUMERICALDEDUCTIONRUNTIME_API FDebugPrintfLogContainer
{
	void Add(
	const FString& InCharacterNameActive,
	const FString& InCharacterNamePassive,
	const FString& InEventString,
	const FString& InValue);

	void Pop(FSimplePreDebugPrintf& Out);

	bool IsPop();

private:
	TArray<FSimplePreDebugPrintf> Logs;
};

USTRUCT(BlueprintType)
struct SIMPLENUMERICALDEDUCTIONRUNTIME_API FSimpleSelectString
{
	GENERATED_USTRUCT_BODY()

	FSimpleSelectString();

	UPROPERTY(EditAnywhere)
	TArray<FString> SelectStrings;

	UPROPERTY(EditAnywhere)
	FString SelectString;
};

UENUM(BlueprintType)
enum class EActionCharacterEventType :uint8
{
	DAMAGE_EVENT		UMETA(DisplayName = "Damage"),
	TREATMENT_EVENT		UMETA(DisplayName = "Treatment"),
};

USTRUCT(BlueprintType)
struct SIMPLENUMERICALDEDUCTIONRUNTIME_API FDebugCharacterInfo
{
	GENERATED_USTRUCT_BODY()

	FDebugCharacterInfo();

	UPROPERTY(EditAnywhere)
	FSimpleSelectString Key;

	UPROPERTY(EditAnywhere)
	int32 Level;
};

USTRUCT(BlueprintType)
struct SIMPLENUMERICALDEDUCTIONRUNTIME_API FDebugCharactersInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UNumericalAlgorithmExecuteObject> TestAlgorithmObject;

	UPROPERTY(EditAnywhere)
	TArray<FDebugCharacterInfo> CharacterActive;
	
	UPROPERTY(EditAnywhere)
	EActionCharacterEventType EventType;
	
	UPROPERTY(EditAnywhere)
	TArray<FDebugCharacterInfo> CharacterPassive;
};


UENUM(BlueprintType)
enum class EDeduceAttributeDataType :uint8
{
	DEDUCETYPE_INT32  UMETA(DisplayName = "in32"),
	DEDUCETYPE_FLOAT  UMETA(DisplayName = "float"),
	DEDUCETYPE_STRING UMETA(DisplayName = "string"),
};

USTRUCT(BlueprintType)
struct SIMPLENUMERICALDEDUCTIONRUNTIME_API FNAEParam
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Key;

	//经过迭代值
	UPROPERTY(BlueprintReadOnly)
	float Value;

	UPROPERTY(BlueprintReadOnly)
	int32 Count;

	UPROPERTY(BlueprintReadOnly)
	float Coefficient;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FString> AttributeData;
};

USTRUCT(BlueprintType)
struct SIMPLENUMERICALDEDUCTIONRUNTIME_API FDeduceAttributeData
{
	GENERATED_USTRUCT_BODY()

	FDeduceAttributeData();

	UPROPERTY()
	FName Key;

	UPROPERTY()
	float Coefficient;

	UPROPERTY()
	FString Value;

	UPROPERTY()
	TArray<FString> DeduceValue;

	UPROPERTY()
	EDeduceAttributeDataType AttributeDataType;

	UPROPERTY()
	TSubclassOf<UNumericalAlgorithmExecuteObject> BaseAlgorithm;
};

USTRUCT(BlueprintType)
struct SIMPLENUMERICALDEDUCTIONRUNTIME_API FDeduceAttributeDataTables
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FName TableName;
	
	UPROPERTY()
	TArray<FDeduceAttributeData> AttributeDatas;
};
