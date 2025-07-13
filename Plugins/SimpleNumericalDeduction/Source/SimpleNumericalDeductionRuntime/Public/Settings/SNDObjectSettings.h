// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleNumericalDeductionType.h"
#include "SNDObjectSettings.generated.h"

class UDataTable;
class UNumericalAlgorithmExecuteObject;
UCLASS(config = SNDObjectSettings)
class SIMPLENUMERICALDEDUCTIONRUNTIME_API USNDObjectSettings : public UObject
{
	GENERATED_BODY()

public:
	USNDObjectSettings();

	//���ݵĻ�����
	UPROPERTY(EditAnywhere, Category = DeductionSettings, meta = (ToolTip = "This content is version base, which is used for automatic iteration."))
	UDataTable *BaseTable;

	//CSV�洢��λ��
	UPROPERTY(config, EditAnywhere, Category = DeductionSettings, meta = (ToolTip = "This content is version base, which is used for automatic iteration."))
	FDirectoryPath CSVSavePath;

	//���ݵĴ���
	UPROPERTY(config, EditAnywhere, Category = DeductionSettings, meta = (ToolTip = "This content is version base, which is used for automatic iteration."))
	int32 DeductionNumber;

	UPROPERTY(config, EditAnywhere, Category = AlgorithmExecuteObject, meta = (ToolTip = "This content is version base, which is used for automatic iteration."))
	TSubclassOf<UNumericalAlgorithmExecuteObject> NumericalAlgorithmExecuteObject;

	//�㷨ģ��
	UPROPERTY(config, EditAnywhere, Category = AlgorithmExecuteObject, meta = (ToolTip = "This content is version base, which is used for automatic iteration."))
	TArray<TSubclassOf<UNumericalAlgorithmExecuteObject>> NumericalAlgorithmExecuteObjects;

	UPROPERTY(config, EditAnywhere, Category = Coefficient, meta = (ToolTip = "This content is version base, which is used for automatic iteration."))
	float Coefficient;

	UPROPERTY(config, EditAnywhere, Category = Coefficient, meta = (ToolTip = "This content is version base, which is used for automatic iteration."))
	TArray<float> Coefficients;

	//���ɵľ�����������
	UPROPERTY(config, VisibleAnywhere, Category = InvisibleData, meta = (ToolTip = "This content is version base, which is used for automatic iteration."))
	TArray<FDeduceAttributeDataTables> AttributeDatas;
public:
	bool AnalysisBaseTable();

	void SaveObjectConfig();
	void LoadObjectConfig();

	FString GetDefaultConfigFilename();
};