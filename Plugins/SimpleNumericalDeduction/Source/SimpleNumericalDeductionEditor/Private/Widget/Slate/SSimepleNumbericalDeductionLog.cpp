// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "Widget/Slate/SSimepleNumbericalDeductionLog.h"
#include "Widgets/Layout/SBox.h"
#include "Widget/Slate/SlateElement/SAttributeLogBase.h"
#include "Log/SimpleNumericalDeductionLog.h"
#include "Settings/SNDNumericalBalanceDebugSettings.h"
#include "Settings/SNDObjectSettings.h"
#include "UObject/NumericalAlgorithmExecuteObject.h"

#define LOCTEXT_NAMESPACE "SSimepleNumbericalDeductionLog"

void SSimepleNumbericalDeductionLog::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SAssignNew(BoxList, SVerticalBox)
	];
}

bool SSimepleNumbericalDeductionLog::IsTickableInEditor() const
{
	return true;
}

void SSimepleNumbericalDeductionLog::Tick(float DeltaTime)
{
	if (LogContainer.IsPop())
	{
		FSimplePreDebugPrintf MyLog;
		LogContainer.Pop(MyLog);

		AddLog(MyLog);
	}
}

TStatId SSimepleNumbericalDeductionLog::GetStatId() const
{
	return TStatId();
}

void SSimepleNumbericalDeductionLog::Generate()
{
	//测试 方便查看
	//for (size_t i = 0; i < 1000; i++)
	//{
	//	FSimplePreDebugPrintf PrintfLog;
	//	PrintfLog.CharacterNameActive = TEXT("小明");
	//	PrintfLog.CharacterNamePassive = TEXT("小王");
	//	PrintfLog.EventString = TEXT("造成伤害");
	//	PrintfLog.Value = TEXT("2000");

	//	AddLog(PrintfLog);
	//}

	if (const USNDObjectSettings* SNDObjectSettings = GetDefault<USNDObjectSettings>())
	{
		//寻找对象数据
		auto FindData = [&](const FString &InKey)->const TArray<FDeduceAttributeData>*
		{
			for (auto &Tmp : SNDObjectSettings->AttributeDatas)
			{
				if (Tmp.TableName.ToString() == InKey)
				{
					return &Tmp.AttributeDatas;
				}
			}

			return NULL;
		};

		//寻找对应等级下的数据
		auto GetSpecifyLevelData = [](
			int32 InLv,
			const TArray<FDeduceAttributeData>* InDeduceAttributeData,
			const FDebugCharacterInfo& InDebugCharacterInfo,
			TMap<FName, float>& OutLvData)
		{
			for (auto& TmpActiveData : *InDeduceAttributeData)
			{
				if (TmpActiveData.DeduceValue.IsValidIndex(InLv))
				{
					FString InKey = TmpActiveData.Key.ToString();
					InKey.RemoveSpacesInline();//移除多余空格

					OutLvData.Add(*InKey, FCString::Atof(*TmpActiveData.DeduceValue[InLv - 1]));
				}
				else
				{

					return;
				}
			}
		};

		auto GetValueFromMap = [](const FName& InName,const TMap<FName, float>& InMapData)->float
		{
			if (const float *InValue = InMapData.Find(InName))
			{
				return *InValue;
			}

			return -1.f;
		};

		//模拟策略
		auto SimulationCalculation = [&](
			TSubclassOf<UNumericalAlgorithmExecuteObject> InTestAlgorithmObject,
			const FString &InActiveCharacterName,
			const FString& InPassiveCharacterName,
			EActionCharacterEventType EventType,
			const TMap<FName, float> &InLvActiveData,
			const TMap<FName, float> &InLvPassiveData)
		{
			if (UNumericalAlgorithmExecuteObject* InObject =
				Cast<UNumericalAlgorithmExecuteObject>(InTestAlgorithmObject->GetDefaultObject()))
			{
				auto GetEventTypeString = [](EActionCharacterEventType InEventType)->FString
				{
					switch (InEventType)
					{
					case EActionCharacterEventType::DAMAGE_EVENT:
						return TEXT("--造成伤害--");
					case EActionCharacterEventType::TREATMENT_EVENT:
						return TEXT("--提供治疗--");
					}

					return TEXT("-出错-");
				};

				auto GetEventTypeValue = [&](EActionCharacterEventType InEventType)->float
				{
					switch (InEventType)
					{
					case EActionCharacterEventType::DAMAGE_EVENT:
						return InObject->GetDamageAlgorithmValue(InLvActiveData, InLvPassiveData);
					case EActionCharacterEventType::TREATMENT_EVENT:
						return InObject->GetTreatmentAlgorithmValue(InLvActiveData, InLvPassiveData);
					}

					return 0.f;
				};

				int32 ActiveLevel = GetValueFromMap(TEXT("Level"), InLvActiveData);
				int32 PassiveLevel = GetValueFromMap(TEXT("Level"), InLvPassiveData);

				FSimplePreDebugPrintf PrintfLog;
				PrintfLog.CharacterNameActive = FString::Printf(TEXT("Lv %i %s"), ActiveLevel,*InActiveCharacterName);
				PrintfLog.CharacterNamePassive = FString::Printf(TEXT("Lv %i %s"), PassiveLevel, *InPassiveCharacterName);
				PrintfLog.EventString = GetEventTypeString(EventType);
				PrintfLog.Value = FString::Printf(TEXT("[%.2f]"), GetEventTypeValue(EventType));

				AddLog(PrintfLog);
			}
		};

		if (const USNDNumericalBalanceDebugSettings* SNDNumericalBalanceDebugSettings = GetDefault<USNDNumericalBalanceDebugSettings>())
		{
			for (auto &Tmp : SNDNumericalBalanceDebugSettings->DebugCharactersInfo)
			{
				if (!SNDNumericalBalanceDebugSettings->bIterationLevel)//非迭代，自动的
				{
					for (auto& TmpActive : Tmp.CharacterActive)
					{
						for (auto& TmpPassive : Tmp.CharacterPassive)
						{
							if (const TArray<FDeduceAttributeData>* Active = FindData(TmpActive.Key.SelectString))
							{
								if (const TArray<FDeduceAttributeData>* Passive = FindData(TmpPassive.Key.SelectString))
								{
									TMap<FName, float> LvActiveData;
									GetSpecifyLevelData(TmpActive.Level,Active, TmpActive, LvActiveData);

									TMap<FName, float> LvPassiveData;
									GetSpecifyLevelData(TmpPassive.Level,Passive, TmpPassive, LvPassiveData);

									//开始模拟计算
									SimulationCalculation(Tmp.TestAlgorithmObject,
										TmpActive.Key.SelectString,
										TmpPassive.Key.SelectString,
										Tmp.EventType,
										LvActiveData, 
										LvPassiveData);
								}
							}
						}
					}
				}
				else
				{
					switch (SNDNumericalBalanceDebugSettings->IterativeMethod)
					{
						case EIterativeDebugPrintMethod::ONE_TO_MANY_METHOD:
						{
							for (auto& TmpActive : Tmp.CharacterActive)
							{
								if (const TArray<FDeduceAttributeData>* Active = FindData(TmpActive.Key.SelectString))
								{
									TMap<FName, float> LvActiveData;
									GetSpecifyLevelData(TmpActive.Level, Active, TmpActive, LvActiveData);

									for (auto& TmpPassive : Tmp.CharacterPassive)
									{
										if (const TArray<FDeduceAttributeData>* Passive = FindData(TmpPassive.Key.SelectString))
										{
											for (int32 i = 1; i <= SNDObjectSettings->DeductionNumber; i++)
											{
												TMap<FName, float> LvPassiveData;
												GetSpecifyLevelData(i, Passive, TmpPassive, LvPassiveData);

												SimulationCalculation(Tmp.TestAlgorithmObject,
													TmpActive.Key.SelectString,
													TmpPassive.Key.SelectString,
													Tmp.EventType,
													LvActiveData,
													LvPassiveData);
											}
										}
									}
								}
							}

							break;
						}
						case EIterativeDebugPrintMethod::MANY_TO_MANY_METHOD:
						{
							for (auto& TmpActive : Tmp.CharacterActive)
							{
								if (const TArray<FDeduceAttributeData>* Active = FindData(TmpActive.Key.SelectString))
								{
									for (auto& TmpPassive : Tmp.CharacterPassive)
									{
										if (const TArray<FDeduceAttributeData>* Passive = FindData(TmpPassive.Key.SelectString))
										{
											for (int32 i = 1; i <= SNDObjectSettings->DeductionNumber; i++)
											{
												TMap<FName, float> LvActiveData;
												GetSpecifyLevelData(i, Active, TmpActive, LvActiveData);

												TMap<FName, float> LvPassiveData;
												GetSpecifyLevelData(i, Passive, TmpPassive, LvPassiveData);

												SimulationCalculation(Tmp.TestAlgorithmObject,
													TmpActive.Key.SelectString,
													TmpPassive.Key.SelectString,
													Tmp.EventType,
													LvActiveData,
													LvPassiveData);
											}
										}
									}
								}
							}

							break;
						}
						case EIterativeDebugPrintMethod::MANY_TO_ONCE_METHOD:
						{
							for (auto& TmpPassive : Tmp.CharacterPassive)
							{
								if (const TArray<FDeduceAttributeData>* Passive = FindData(TmpPassive.Key.SelectString))
								{
									TMap<FName, float> LvPassiveData;
									GetSpecifyLevelData(TmpPassive.Level , Passive, TmpPassive, LvPassiveData);

									for (auto& TmpActive : Tmp.CharacterActive)
									{
										if (const TArray<FDeduceAttributeData>* Active = FindData(TmpPassive.Key.SelectString))
										{
											for (int32 i = 1; i <= SNDObjectSettings->DeductionNumber; i++)
											{
												TMap<FName, float> LvActiveData;
												GetSpecifyLevelData(i, Active, TmpActive, LvActiveData);
												
												SimulationCalculation(Tmp.TestAlgorithmObject,
													TmpActive.Key.SelectString,
													TmpPassive.Key.SelectString,
													Tmp.EventType,
													LvActiveData,
													LvPassiveData);
											}
										}
									}
								}
							}

							break;
						}
					}
				}
			}
		}
	}
}

void SSimepleNumbericalDeductionLog::AddLog(const FSimplePreDebugPrintf &MyLog)
{
	if (BoxList.IsValid())
	{
		BoxList->AddSlot()
		.Padding(4.f, 2.f, 4.f, 2.f)
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.AutoHeight()
		[
			SNew(SAttributeLogBase,MyLog)
		];
	}
}

void SSimepleNumbericalDeductionLog::ClearLog()
{
	if (BoxList.IsValid())
	{
		BoxList->ClearChildren();
	}
}

void SSimepleNumbericalDeductionLog::SaveAsText()
{

}

#undef LOCTEXT_NAMESPACE