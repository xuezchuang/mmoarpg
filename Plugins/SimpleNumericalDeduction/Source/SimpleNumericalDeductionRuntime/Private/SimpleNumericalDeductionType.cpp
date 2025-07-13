// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "SimpleNumericalDeductionType.h"
#include "UObject/NumericalAlgorithmExecuteObject.h"
#include "Settings/SNDObjectSettings.h"

FDeduceAttributeData::FDeduceAttributeData()
	:Coefficient(0.f)
	, BaseAlgorithm(UNumericalAlgorithmExecuteObject::StaticClass())
{}

FSimpleSelectString::FSimpleSelectString()
{
	if (USNDObjectSettings* SND = const_cast<USNDObjectSettings*>(GetDefault<USNDObjectSettings>()))
	{
		for (auto &Tmp : SND->AttributeDatas)
		{
			SelectStrings.Add(Tmp.TableName.ToString());
		}
	}
}

void FDebugPrintfLogContainer::Add(const FString& InCharacterNameActive, const FString& InCharacterNamePassive, const FString& InEventString, const FString& InValue)
{
	FSimplePreDebugPrintf &InLog = Logs.AddDefaulted_GetRef();

	InLog.CharacterNameActive = InCharacterNameActive;
	InLog.CharacterNamePassive = InCharacterNamePassive;
	InLog.EventString = InEventString;
	InLog.Value = InValue;
}

void FDebugPrintfLogContainer::Pop(FSimplePreDebugPrintf& Out)
{
	Out = Logs.Pop();
}

bool FDebugPrintfLogContainer::IsPop()
{
	return Logs.Num() != 0;
}

FDebugCharacterInfo::FDebugCharacterInfo()
	:Level(1)
{

}