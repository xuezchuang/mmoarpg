// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "Settings/SNDObjectSettings.h"
#include "HAL/PlatformFilemanager.h"
#include "Engine/DataTable.h"
#include "DataTableUtils.h"
#include "UObject/UnrealType.h"

USNDObjectSettings::USNDObjectSettings()
	:BaseTable(NULL)
{
	CSVSavePath.Path = FPaths::ProjectSavedDir() / TEXT("GameplayAbilitiesCSV");

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*CSVSavePath.Path))
	{
		PlatformFile.CreateDirectory(*CSVSavePath.Path);
	}
}

bool USNDObjectSettings::AnalysisBaseTable()
{
	if (BaseTable)
	{
		AttributeDatas.Empty();

		TMap<FName, uint8*>& RowMap = const_cast<TMap<FName, uint8*>&>(BaseTable->GetRowMap());
		if (const UScriptStruct* RowStruct = BaseTable->GetRowStruct())
		{
			FDeduceAttributeDataTables TmpAttributeDataTables;

			//I 提取结构的变量名称
			TArray<FProperty*> StructProps;
			for (TFieldIterator<FProperty> It(RowStruct); It; ++It)
			{
				FProperty* Prop = *It;
				check(Prop != nullptr);
				StructProps.Add(Prop);
			}

			for (int32 i = 0; i < StructProps.Num(); i++)
			{
				FDeduceAttributeData& InAttributeData = TmpAttributeDataTables.AttributeDatas.AddDefaulted_GetRef();
#if WITH_EDITORONLY_DATA	
				//需要用这个,否则蓝图名称支持不正确
				InAttributeData.Key = *(StructProps[i]->GetDisplayNameText().ToString());
#endif
			}

			//II 提取值
			for (auto RowIt = RowMap.CreateConstIterator(); RowIt; ++RowIt)
			{
				FDeduceAttributeDataTables& InAttributeData = AttributeDatas.Add_GetRef(TmpAttributeDataTables);
				InAttributeData.TableName = RowIt.Key();

				uint8*RowData = RowIt.Value();
				for (int32 PropIdx = 0; PropIdx < StructProps.Num(); PropIdx++)
				{
					InAttributeData.AttributeDatas[PropIdx].Value = DataTableUtils::GetPropertyValueAsString(StructProps[PropIdx], RowData, EDataTableExportFlags::None);
					
					//类型收集
					if (StructProps[PropIdx]->IsA(FIntProperty::StaticClass()) ||
						StructProps[PropIdx]->IsA(FInt8Property::StaticClass()) ||
						StructProps[PropIdx]->IsA(FInt64Property::StaticClass()))
					{
						InAttributeData.AttributeDatas[PropIdx].AttributeDataType = EDeduceAttributeDataType::DEDUCETYPE_INT32;
					}
					else if (StructProps[PropIdx]->IsA(FFloatProperty::StaticClass()) ||
						StructProps[PropIdx]->IsA(FDoubleProperty::StaticClass()))
					{
						InAttributeData.AttributeDatas[PropIdx].AttributeDataType = EDeduceAttributeDataType::DEDUCETYPE_FLOAT;
					}
					else if (StructProps[PropIdx]->IsA(FStrProperty::StaticClass()) ||
						StructProps[PropIdx]->IsA(FTextProperty::StaticClass()) ||
						StructProps[PropIdx]->IsA(FNameProperty::StaticClass()) ||
						StructProps[PropIdx]->IsA(FArrayProperty::StaticClass()))
					{
						InAttributeData.AttributeDatas[PropIdx].AttributeDataType = EDeduceAttributeDataType::DEDUCETYPE_STRING;
					}
					else
					{
						InAttributeData.AttributeDatas[PropIdx].AttributeDataType = EDeduceAttributeDataType::DEDUCETYPE_FLOAT;
					}
				}
			}

			return true;
		}
	}

	return false;
}

FString USNDObjectSettings::GetDefaultConfigFilename()
{
	return FPaths::ProjectConfigDir() / TEXT("Default") + GetClass()->ClassConfigName.ToString() + TEXT(".ini");
}

void USNDObjectSettings::SaveObjectConfig()
{
	SaveConfig(CPF_Config, *GetDefaultConfigFilename());
}

void USNDObjectSettings::LoadObjectConfig()
{
	LoadConfig(GetClass(), *GetDefaultConfigFilename());
}
