#include "MMOARPGType.h"
#include "Json.h"

void FMMOARPGPlayerRegistInfo::Reset()
{
	UserInfo.Reset();
	CAInfo.Reset();
	CharacterAttributes.Empty();
	UserState.Reset();
}

bool FMMOARPGPlayerRegistInfo::IsValid()
{
	return UserInfo.ID != INDEX_NONE;
}

void FMMOARPGCharacterAppearance::Reset()
{
	Lv = INDEX_NONE;
	SlotPosition = INDEX_NONE;
	LegSize = 0.0f;
	WaistSize = 0.0f;
	ArmSize = 0.0f;

	Name.Empty();
	Date.Empty();
}

bool FDicatedServerControlRange::IsWithinRange(const FVector& InNewPoint)
{
	return true;
	return InNewPoint.X <= UpperRight.X && InNewPoint.X >= LowerLeft.X &&
		   InNewPoint.Y <= LowerLeft.Y && InNewPoint.Y >= UpperRight.Y;
}

namespace NetDataAnalysis
{
	void AnalysisToArrayName(const FString& InRowString, TArray<FName>& OutInfo)
	{
		TArray<FString> InfoArray;
		InRowString.ParseIntoArray(InfoArray, TEXT("|"));

		for (auto &Tmp : InfoArray)
		{
			OutInfo.Add(*Tmp);
		}
	}

	void UserDataToString(const FMMOARPGUserData& InUserData, FString& OutString)
	{
		OutString.Empty();

		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutString);

		JsonWriter->WriteObjectStart();

		JsonWriter->WriteValue(TEXT("ID"), InUserData.ID);
		//JsonWriter->WriteValue(TEXT("Account"), InUserData.Account);
		//JsonWriter->WriteValue(TEXT("EMail"), InUserData.EMail);
		//JsonWriter->WriteValue(TEXT("Name"), InUserData.Name);
		//JsonWriter->WriteValue(TEXT("HeadPortraitURL"), InUserData.HeadPortraitURL);

		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();
	}

	void StringToUserData(const FString& JsonString, FMMOARPGUserData& OutUserData)
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
		TSharedPtr<FJsonObject> ReadRoot;

		if (FJsonSerializer::Deserialize(JsonReader, ReadRoot))
		{
			OutUserData.ID = ReadRoot->GetIntegerField(TEXT("ID"));
			//OutUserData.Account = ReadRoot->GetStringField(TEXT("Account"));
			//OutUserData.Name = ReadRoot->GetStringField(TEXT("Name"));
			//OutUserData.EMail = ReadRoot->GetStringField(TEXT("EMail"));
			//OutUserData.HeadPortraitURL = ReadRoot->GetStringField(TEXT("HeadPortraitURL"));
		}
	}

	void CharacterAppearancesToString(const FCharacterAppearances& InCAs, FString& OutString)
	{
		OutString.Empty();

		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutString);

		JsonWriter->WriteArrayStart();
		for(auto &Tmp : InCAs)
		{
			JsonWriter->WriteObjectStart();

			JsonWriter->WriteValue(TEXT("Name"),Tmp.Name);
			JsonWriter->WriteValue(TEXT("Date"), Tmp.Date);
			JsonWriter->WriteValue(TEXT("Lv"), Tmp.Lv);
			JsonWriter->WriteValue(TEXT("SlotPosition"), Tmp.SlotPosition);
			JsonWriter->WriteValue(TEXT("LegSize"), Tmp.LegSize);
			JsonWriter->WriteValue(TEXT("WaistSize"), Tmp.WaistSize);
			JsonWriter->WriteValue(TEXT("ArmSize"), Tmp.ArmSize);
			JsonWriter->WriteValue(TEXT("ArmSize"), Tmp.ArmSize);
			JsonWriter->WriteValue(TEXT("HeadSize"), Tmp.HeadSize);
			JsonWriter->WriteValue(TEXT("ChestSize"), Tmp.ChestSize);

			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
		JsonWriter->Close();
	}

	void CharacterAppearancesToString(const FMMOARPGCharacterAppearance& InCAs, FString& OutString)
	{
		OutString.Empty();

		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutString);

		JsonWriter->WriteObjectStart();
		{
			JsonWriter->WriteValue(TEXT("Name"), InCAs.Name);
			JsonWriter->WriteValue(TEXT("Date"), InCAs.Date);
			JsonWriter->WriteValue(TEXT("Lv"), InCAs.Lv);
			JsonWriter->WriteValue(TEXT("SlotPosition"), InCAs.SlotPosition);
			JsonWriter->WriteValue(TEXT("LegSize"), InCAs.LegSize);
			JsonWriter->WriteValue(TEXT("WaistSize"), InCAs.WaistSize);
			JsonWriter->WriteValue(TEXT("ArmSize"), InCAs.ArmSize);
			JsonWriter->WriteValue(TEXT("HeadSize"), InCAs.HeadSize);
			JsonWriter->WriteValue(TEXT("ChestSize"), InCAs.ChestSize);
		}
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();
	}

	void StringToCharacterAppearances(const FString& OutString, FCharacterAppearances& InCAs)
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(OutString);
		TArray<TSharedPtr<FJsonValue>> ReadRoot;

		if (FJsonSerializer::Deserialize(JsonReader, ReadRoot))
		{
			for (auto &Tmp : ReadRoot)
			{
				InCAs.Add(FMMOARPGCharacterAppearance());
				FMMOARPGCharacterAppearance &InLast = InCAs.Last();
				if (TSharedPtr<FJsonObject> InJsonObject = Tmp->AsObject())
				{
					InLast.Name = InJsonObject->GetStringField(TEXT("Name"));
					InLast.Date = InJsonObject->GetStringField(TEXT("Date"));
					InLast.Lv = InJsonObject->GetIntegerField(TEXT("Lv"));
					InLast.SlotPosition = InJsonObject->GetIntegerField(TEXT("SlotPosition"));
					InLast.LegSize = InJsonObject->GetNumberField(TEXT("LegSize"));
					InLast.WaistSize = InJsonObject->GetNumberField(TEXT("WaistSize"));
					InLast.ArmSize = InJsonObject->GetNumberField(TEXT("ArmSize"));
					InLast.HeadSize = InJsonObject->GetNumberField(TEXT("HeadSize"));
					InLast.ChestSize = InJsonObject->GetNumberField(TEXT("ChestSize"));
				}
			}
		}
	}

	void StringToCharacterAppearances(const FString& OutString, FMMOARPGCharacterAppearance& InCAs)
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(OutString);
		TSharedPtr<FJsonObject> ReadRoot;

		if (FJsonSerializer::Deserialize(JsonReader, ReadRoot))
		{
			InCAs.Name = ReadRoot->GetStringField(TEXT("Name"));
			InCAs.Date = ReadRoot->GetStringField(TEXT("Date"));
			InCAs.Lv = ReadRoot->GetIntegerField(TEXT("Lv"));
			InCAs.SlotPosition = ReadRoot->GetIntegerField(TEXT("SlotPosition"));
			InCAs.LegSize = ReadRoot->GetNumberField(TEXT("LegSize"));
			InCAs.WaistSize = ReadRoot->GetNumberField(TEXT("WaistSize"));
			InCAs.ArmSize = ReadRoot->GetNumberField(TEXT("ArmSize"));
			InCAs.HeadSize = ReadRoot->GetNumberField(TEXT("HeadSize"));
			InCAs.ChestSize = ReadRoot->GetNumberField(TEXT("ChestSize"));
		}
	}

	void JsonObjectToAttributeData(
		const FString &InJsonObjectName,
		TSharedPtr<FJsonObject> InObjectJson, 
		FMMOARPGAttributeData& InAttributeData)
	{
		if (TSharedPtr<FJsonObject> SubJsonObject = InObjectJson->GetObjectField(InJsonObjectName))
		{
			InAttributeData.BaseValue = SubJsonObject->GetNumberField(TEXT("BaseValue"));
			InAttributeData.CurrentValue = SubJsonObject->GetNumberField(TEXT("CurrentValue"));
		}
	}

	void JsonObjectToAttributeData(
		const FString& InJsonObjectName,
		TSharedPtr<FJsonObject> InObjectJson,
		TArray<FName>& InAttributeData)
	{
		//1|2|4|5
		AnalysisToArrayName(InObjectJson->GetStringField(InJsonObjectName), InAttributeData);
	}

	void AttributeDataToJsonObject(
		const FString& InJsonObjectName, 
		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter,
		const FMMOARPGAttributeData& InAttributeData)
	{
		JsonWriter->WriteObjectStart(InJsonObjectName);
		{
			JsonWriter->WriteValue(TEXT("BaseValue"), InAttributeData.BaseValue);
			JsonWriter->WriteValue(TEXT("CurrentValue"), InAttributeData.CurrentValue);
		}
		JsonWriter->WriteObjectEnd();
	}

	void AttributeDataToJsonObject(
		const FString& InJsonObjectName,
		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter,
		const TArray<FName>& InAttributeData)
	{
		FString ValueString;
		for (auto &Tmp : InAttributeData)
		{
			ValueString += Tmp.ToString() + TEXT("|");
		}

		ValueString.RemoveFromEnd(TEXT("|"));

		JsonWriter->WriteValue(InJsonObjectName, ValueString);
	}

	bool StringToMMOARPGCharacterAttribute(const FString& OutString, FMMOARPGCharacterAttribute& InCA)
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(OutString);
		TSharedPtr<FJsonObject> ReadRoot;

		if (FJsonSerializer::Deserialize(JsonReader, ReadRoot))
		{
			JsonObjectToAttributeData(TEXT("Level"), ReadRoot, InCA.Level);
			JsonObjectToAttributeData(TEXT("Health"), ReadRoot, InCA.Health);
			JsonObjectToAttributeData(TEXT("MaxHealth"), ReadRoot, InCA.MaxHealth);
			JsonObjectToAttributeData(TEXT("Mana"), ReadRoot, InCA.Mana);
			JsonObjectToAttributeData(TEXT("MaxMana"), ReadRoot, InCA.MaxMana);
			JsonObjectToAttributeData(TEXT("PhysicsAttack"), ReadRoot, InCA.PhysicsAttack);
			JsonObjectToAttributeData(TEXT("MagicAttack"), ReadRoot, InCA.MagicAttack);
			JsonObjectToAttributeData(TEXT("PhysicsDefense"), ReadRoot, InCA.PhysicsDefense);
			JsonObjectToAttributeData(TEXT("MagicDefense"), ReadRoot, InCA.MagicDefense);
			JsonObjectToAttributeData(TEXT("AttackRange"), ReadRoot, InCA.AttackRange);
			JsonObjectToAttributeData(TEXT("EmpiricalValue"), ReadRoot, InCA.EmpiricalValue);
			JsonObjectToAttributeData(TEXT("MaxEmpiricalValue"), ReadRoot, InCA.MaxEmpiricalValue);

			JsonObjectToAttributeData(TEXT("ComboAttack"), ReadRoot, InCA.ComboAttack.Slots);
			JsonObjectToAttributeData(TEXT("Skill"), ReadRoot, InCA.Skill.Slots);
			JsonObjectToAttributeData(TEXT("Limbs"), ReadRoot, InCA.Limbs.Slots);

			//读取装配的技能
			InCA.SkillAssemblyString = ReadRoot->GetStringField(TEXT("SkillAssemblyString"));

			return true;
		}

		return false;
	}

	void MMOARPGCharacterAttributeToString(const FMMOARPGCharacterAttribute& InCA, FString& OutString)
	{
		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutString);

		JsonWriter->WriteObjectStart();
		{
			AttributeDataToJsonObject(TEXT("Level"), JsonWriter, InCA.Level);
			AttributeDataToJsonObject(TEXT("Health"), JsonWriter, InCA.Health);
			AttributeDataToJsonObject(TEXT("MaxHealth"), JsonWriter, InCA.MaxHealth);
			AttributeDataToJsonObject(TEXT("Mana"), JsonWriter, InCA.Mana);
			AttributeDataToJsonObject(TEXT("MaxMana"), JsonWriter, InCA.MaxMana);
			AttributeDataToJsonObject(TEXT("PhysicsAttack"), JsonWriter, InCA.PhysicsAttack);
			AttributeDataToJsonObject(TEXT("MagicAttack"), JsonWriter, InCA.MagicAttack);
			AttributeDataToJsonObject(TEXT("PhysicsDefense"), JsonWriter, InCA.PhysicsDefense);
			AttributeDataToJsonObject(TEXT("MagicDefense"), JsonWriter, InCA.MagicDefense);
			AttributeDataToJsonObject(TEXT("AttackRange"), JsonWriter, InCA.AttackRange);
			AttributeDataToJsonObject(TEXT("EmpiricalValue"), JsonWriter, InCA.EmpiricalValue);
			AttributeDataToJsonObject(TEXT("MaxEmpiricalValue"), JsonWriter, InCA.MaxEmpiricalValue);

			AttributeDataToJsonObject(TEXT("ComboAttack"), JsonWriter, InCA.ComboAttack.Slots);
			AttributeDataToJsonObject(TEXT("Skill"), JsonWriter, InCA.Skill.Slots);
			AttributeDataToJsonObject(TEXT("Limbs"), JsonWriter, InCA.Limbs.Slots);

			//写入装配信息
			JsonWriter->WriteValue(TEXT("SkillAssemblyString"), InCA.SkillAssemblyString);
		}

		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();
	}

	bool StringToMMOARPGCharacterAttribute(const FString& OutString, TMap<int32, FMMOARPGCharacterAttribute>& InCA)
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(OutString);
		TArray<TSharedPtr<FJsonValue>> ReadRoot;

		if (FJsonSerializer::Deserialize(JsonReader, ReadRoot))
		{
			for (auto& Tmp : ReadRoot)
			{
				if (TSharedPtr<FJsonObject> InJsonObject = Tmp->AsObject())
				{
					int32 ID = InJsonObject->GetIntegerField(TEXT("ID"));
					FString CharacterAttributeJson = InJsonObject->GetStringField(TEXT("CA"));

					InCA.Add(ID, FMMOARPGCharacterAttribute());
					StringToMMOARPGCharacterAttribute(CharacterAttributeJson,InCA[ID]);
				}
			}

			return true;
		}

		return false;
	}

	void MMOARPGCharacterAttributeToString(const TMap<int32, FMMOARPGCharacterAttribute>& InCA, FString& OutString)
	{
		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutString);

		JsonWriter->WriteArrayStart();
		for (auto& Tmp : InCA)
		{
			JsonWriter->WriteObjectStart();
			{
				JsonWriter->WriteValue(TEXT("ID"), Tmp.Key);

				FString CharacterAttributeString;
				MMOARPGCharacterAttributeToString(Tmp.Value, CharacterAttributeString);

				JsonWriter->WriteValue(TEXT("CA"), CharacterAttributeString);

			}
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
		JsonWriter->Close();
	}

	bool StringToMMOARPGAttributeSlot(const FString& IntString, FMMOARPGAttributeSlot& OutAS)
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(IntString);
		TArray<TSharedPtr<FJsonValue>> ReadRoot;

		if (FJsonSerializer::Deserialize(JsonReader, ReadRoot))
		{
			for (auto& Tmp : ReadRoot)
			{
				OutAS.Slots.Add(*Tmp->AsString());
			}

			return true;
		}

		return false;
	}

	void MMOARPGAttributeSlotToString(const FMMOARPGAttributeSlot& InAS, FString& OutString)
	{
		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutString);

		JsonWriter->WriteArrayStart();
		for (auto& Tmp : InAS.Slots)
		{
			JsonWriter->WriteValue(Tmp.ToString());
		}

		JsonWriter->WriteArrayEnd();
		JsonWriter->Close();
	}
}

void FMMOARPGUserData::Reset()
{
	//ID = INDEX_NONE;

	//EMail.Empty();
	//Name.Empty();
	//HeadPortraitURL.Empty();
	FMemory::Memset(&Account, 0, sizeof(Account));
	FMemory::Memset(&loingkey, 0, sizeof(loingkey));
}

void FMMOARPGCharacterAttribute::Clear()
{
	Limbs.Slots.Empty();
	Skill.Slots.Empty();
	ComboAttack.Slots.Empty();
}

FString FMMOARPGAttributeSlot::ToString() const
{
	TCHAR* InSplitSymbol = TEXT("|");

	FString String;
	for (auto& Tmp : Slots)
	{
		String += Tmp.ToString() + InSplitSymbol;
	}
	String.RemoveFromEnd(InSplitSymbol);

	if (String.IsEmpty())
	{
		String = TEXT("0");
	}

	return String;
}

FMMOARPGUserState::FMMOARPGUserState()
{
	Reset();
}

void FMMOARPGUserState::Reset()
{
	Position = FVector::ZeroVector;
	Rotator = FRotator::ZeroRotator;
	ForwardVector = FVector::ZeroVector;
	DSID = INDEX_NONE;
	CharacterActionState = ECharacterActionState::NORMAL_STATE;
	//DSKey.IP = 0;
	//DSKey.Port = 0;

	CharacterID = 0;
}

FString FMMOARPGUserState::PositionToString()const
{
	return FString::Printf(TEXT("%.2lf,%.2lf,%.2lf"),
		Position.X,
		Position.Y,
		Position.Z);
}

FString FMMOARPGUserState::RotatorToString()const
{
	return FString::Printf(TEXT("%.2lf,%.2lf,%.2lf"),
		Rotator.Pitch,
		Rotator.Yaw,
		Rotator.Roll);
}

void FMMOARPGUserState::Init(const FMMOARPGUserState& InUserState)
{
	Position = InUserState.Position;
	Rotator = InUserState.Rotator;
	ForwardVector = InUserState.ForwardVector;
	CharacterActionState = InUserState.CharacterActionState;
	CharacterID = InUserState.CharacterID;
}
