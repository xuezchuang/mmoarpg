
#pragma once
#include "CoreMinimal.h"

//The file used to reflect gameplay tags is used.


/*A static function that contains a list of types order*/
int32 GetGamePlayTagOrderIndex(const FName &InTag)
{
	static TArray<FString> GameplayTagsOrder = 
	{
		"Character.Skill.Dodge",
		"Character.Skill.Sprint",
		"Character.State.Die",
		"Character.State.Hit",
		"Character.Buff.Damage",
		"Character.Skill.WhirlwindBlow",
		"Character.Skill.SprintWhirlwindBlow",
		"Character.Skill.EscapeToKill",
		"Character.Skill.StreamBlade",
		"Character.Cooldown.Skill",
		"Character.Skill.Spike",
		"Character.Buff.Recovery.Health",
		"Character.Buff.Recovery.Mana",
		"Character.Buff.Recovery.Stamina",
		"Character.Skill.PickFly",
		"Character.Attack.ComboLinkage.Air",
		"Character.Attack.ComboLinkage.Ground",
		"Character.Skill.SprintPickFly",
		"GameplayCue.Hit",
		"Character.Skill.AVolley",
		"Character.Attack.ComboLinkage.Ground.Type2",
		"Character.Skill.FlameCutDown",
		"Character.Skill.WhirlwindProvoked",
		"Character.Buff.SuperArmor",
		"Character.State.Stun",
		"Character.Skill.Block",
		"Character.Skill.Block.Type1",
		"Character.Skill.ShieldAtk",
		"GameplayCue.Skill.Shield",
		"Character.Buff.Shield",
		"Character.Skill.WirelessShooting",
	};

	return GameplayTagsOrder.Find(InTag.ToString());
}

/*Get the number of enumerations of the current order*/
int32 GetOrderMatchingEnumIndex(int32 InGameplayTagsOrderIndex)
{
	if (InGameplayTagsOrderIndex == 0)
	{
		return 1;
	}

	return FMath::CeilToInt((float)InGameplayTagsOrderIndex / 63.f);
}

/*Enumerations can be generated dynamically from gameplaytag*/
enum class EGamePlayTags0 :uint64
{
	Character_Skill_Dodge	= 1,
	Character_Skill_Sprint	= 2,
	Character_State_Die	= 4,
	Character_State_Hit	= 8,
	Character_Buff_Damage	= 16,
	Character_Skill_WhirlwindBlow	= 32,
	Character_Skill_SprintWhirlwindBlow	= 64,
	Character_Skill_EscapeToKill	= 128,
	Character_Skill_StreamBlade	= 256,
	Character_Cooldown_Skill	= 512,
	Character_Skill_Spike	= 1024,
	Character_Buff_Recovery_Health	= 2048,
	Character_Buff_Recovery_Mana	= 4096,
	Character_Buff_Recovery_Stamina	= 8192,
	Character_Skill_PickFly	= 16384,
	Character_Attack_ComboLinkage_Air	= 32768,
	Character_Attack_ComboLinkage_Ground	= 65536,
	Character_Skill_SprintPickFly	= 131072,
	GameplayCue_Hit	= 262144,
	Character_Skill_AVolley	= 524288,
	Character_Attack_ComboLinkage_Ground_Type2	= 1048576,
	Character_Skill_FlameCutDown	= 2097152,
	Character_Skill_WhirlwindProvoked	= 4194304,
	Character_Buff_SuperArmor	= 8388608,
	Character_State_Stun	= 16777216,
	Character_Skill_Block	= 33554432,
	Character_Skill_Block_Type1	= 67108864,
	Character_Skill_ShieldAtk	= 134217728,
	GameplayCue_Skill_Shield	= 268435456,
	Character_Buff_Shield	= 536870912,
	Character_Skill_WirelessShooting	= 1073741824,
};

/*Convert gamploytag tag to enumeration*/
EGamePlayTags0  NameToEGamePlayTags0(const FName &InName)
{
	if(InName == TEXT("Character.Skill.Dodge"))
	{
		 return EGamePlayTags0::Character_Skill_Dodge;
	}else
	if(InName == TEXT("Character.Skill.Sprint"))
	{
		 return EGamePlayTags0::Character_Skill_Sprint;
	}else
	if(InName == TEXT("Character.State.Die"))
	{
		 return EGamePlayTags0::Character_State_Die;
	}else
	if(InName == TEXT("Character.State.Hit"))
	{
		 return EGamePlayTags0::Character_State_Hit;
	}else
	if(InName == TEXT("Character.Buff.Damage"))
	{
		 return EGamePlayTags0::Character_Buff_Damage;
	}else
	if(InName == TEXT("Character.Skill.WhirlwindBlow"))
	{
		 return EGamePlayTags0::Character_Skill_WhirlwindBlow;
	}else
	if(InName == TEXT("Character.Skill.SprintWhirlwindBlow"))
	{
		 return EGamePlayTags0::Character_Skill_SprintWhirlwindBlow;
	}else
	if(InName == TEXT("Character.Skill.EscapeToKill"))
	{
		 return EGamePlayTags0::Character_Skill_EscapeToKill;
	}else
	if(InName == TEXT("Character.Skill.StreamBlade"))
	{
		 return EGamePlayTags0::Character_Skill_StreamBlade;
	}else
	if(InName == TEXT("Character.Cooldown.Skill"))
	{
		 return EGamePlayTags0::Character_Cooldown_Skill;
	}else
	if(InName == TEXT("Character.Skill.Spike"))
	{
		 return EGamePlayTags0::Character_Skill_Spike;
	}else
	if(InName == TEXT("Character.Buff.Recovery.Health"))
	{
		 return EGamePlayTags0::Character_Buff_Recovery_Health;
	}else
	if(InName == TEXT("Character.Buff.Recovery.Mana"))
	{
		 return EGamePlayTags0::Character_Buff_Recovery_Mana;
	}else
	if(InName == TEXT("Character.Buff.Recovery.Stamina"))
	{
		 return EGamePlayTags0::Character_Buff_Recovery_Stamina;
	}else
	if(InName == TEXT("Character.Skill.PickFly"))
	{
		 return EGamePlayTags0::Character_Skill_PickFly;
	}else
	if(InName == TEXT("Character.Attack.ComboLinkage.Air"))
	{
		 return EGamePlayTags0::Character_Attack_ComboLinkage_Air;
	}else
	if(InName == TEXT("Character.Attack.ComboLinkage.Ground"))
	{
		 return EGamePlayTags0::Character_Attack_ComboLinkage_Ground;
	}else
	if(InName == TEXT("Character.Skill.SprintPickFly"))
	{
		 return EGamePlayTags0::Character_Skill_SprintPickFly;
	}else
	if(InName == TEXT("GameplayCue.Hit"))
	{
		 return EGamePlayTags0::GameplayCue_Hit;
	}else
	if(InName == TEXT("Character.Skill.AVolley"))
	{
		 return EGamePlayTags0::Character_Skill_AVolley;
	}else
	if(InName == TEXT("Character.Attack.ComboLinkage.Ground.Type2"))
	{
		 return EGamePlayTags0::Character_Attack_ComboLinkage_Ground_Type2;
	}else
	if(InName == TEXT("Character.Skill.FlameCutDown"))
	{
		 return EGamePlayTags0::Character_Skill_FlameCutDown;
	}else
	if(InName == TEXT("Character.Skill.WhirlwindProvoked"))
	{
		 return EGamePlayTags0::Character_Skill_WhirlwindProvoked;
	}else
	if(InName == TEXT("Character.Buff.SuperArmor"))
	{
		 return EGamePlayTags0::Character_Buff_SuperArmor;
	}else
	if(InName == TEXT("Character.State.Stun"))
	{
		 return EGamePlayTags0::Character_State_Stun;
	}else
	if(InName == TEXT("Character.Skill.Block"))
	{
		 return EGamePlayTags0::Character_Skill_Block;
	}else
	if(InName == TEXT("Character.Skill.Block.Type1"))
	{
		 return EGamePlayTags0::Character_Skill_Block_Type1;
	}else
	if(InName == TEXT("Character.Skill.ShieldAtk"))
	{
		 return EGamePlayTags0::Character_Skill_ShieldAtk;
	}else
	if(InName == TEXT("GameplayCue.Skill.Shield"))
	{
		 return EGamePlayTags0::GameplayCue_Skill_Shield;
	}else
	if(InName == TEXT("Character.Buff.Shield"))
	{
		 return EGamePlayTags0::Character_Buff_Shield;
	}else
	if(InName == TEXT("Character.Skill.WirelessShooting"))
	{
		 return EGamePlayTags0::Character_Skill_WirelessShooting;
	}

	return (EGamePlayTags0)0llu; 
}

/*Convert enumeration to gameplaytag.*/
FString  EGamePlayTags0ToName(const EGamePlayTags0 &InTag)
{
	switch(InTag)
	{
		case EGamePlayTags0::Character_Skill_Dodge:
		 return TEXT("Character.Skill.Dodge");

		case EGamePlayTags0::Character_Skill_Sprint:
		 return TEXT("Character.Skill.Sprint");

		case EGamePlayTags0::Character_State_Die:
		 return TEXT("Character.State.Die");

		case EGamePlayTags0::Character_State_Hit:
		 return TEXT("Character.State.Hit");

		case EGamePlayTags0::Character_Buff_Damage:
		 return TEXT("Character.Buff.Damage");

		case EGamePlayTags0::Character_Skill_WhirlwindBlow:
		 return TEXT("Character.Skill.WhirlwindBlow");

		case EGamePlayTags0::Character_Skill_SprintWhirlwindBlow:
		 return TEXT("Character.Skill.SprintWhirlwindBlow");

		case EGamePlayTags0::Character_Skill_EscapeToKill:
		 return TEXT("Character.Skill.EscapeToKill");

		case EGamePlayTags0::Character_Skill_StreamBlade:
		 return TEXT("Character.Skill.StreamBlade");

		case EGamePlayTags0::Character_Cooldown_Skill:
		 return TEXT("Character.Cooldown.Skill");

		case EGamePlayTags0::Character_Skill_Spike:
		 return TEXT("Character.Skill.Spike");

		case EGamePlayTags0::Character_Buff_Recovery_Health:
		 return TEXT("Character.Buff.Recovery.Health");

		case EGamePlayTags0::Character_Buff_Recovery_Mana:
		 return TEXT("Character.Buff.Recovery.Mana");

		case EGamePlayTags0::Character_Buff_Recovery_Stamina:
		 return TEXT("Character.Buff.Recovery.Stamina");

		case EGamePlayTags0::Character_Skill_PickFly:
		 return TEXT("Character.Skill.PickFly");

		case EGamePlayTags0::Character_Attack_ComboLinkage_Air:
		 return TEXT("Character.Attack.ComboLinkage.Air");

		case EGamePlayTags0::Character_Attack_ComboLinkage_Ground:
		 return TEXT("Character.Attack.ComboLinkage.Ground");

		case EGamePlayTags0::Character_Skill_SprintPickFly:
		 return TEXT("Character.Skill.SprintPickFly");

		case EGamePlayTags0::GameplayCue_Hit:
		 return TEXT("GameplayCue.Hit");

		case EGamePlayTags0::Character_Skill_AVolley:
		 return TEXT("Character.Skill.AVolley");

		case EGamePlayTags0::Character_Attack_ComboLinkage_Ground_Type2:
		 return TEXT("Character.Attack.ComboLinkage.Ground.Type2");

		case EGamePlayTags0::Character_Skill_FlameCutDown:
		 return TEXT("Character.Skill.FlameCutDown");

		case EGamePlayTags0::Character_Skill_WhirlwindProvoked:
		 return TEXT("Character.Skill.WhirlwindProvoked");

		case EGamePlayTags0::Character_Buff_SuperArmor:
		 return TEXT("Character.Buff.SuperArmor");

		case EGamePlayTags0::Character_State_Stun:
		 return TEXT("Character.State.Stun");

		case EGamePlayTags0::Character_Skill_Block:
		 return TEXT("Character.Skill.Block");

		case EGamePlayTags0::Character_Skill_Block_Type1:
		 return TEXT("Character.Skill.Block.Type1");

		case EGamePlayTags0::Character_Skill_ShieldAtk:
		 return TEXT("Character.Skill.ShieldAtk");

		case EGamePlayTags0::GameplayCue_Skill_Shield:
		 return TEXT("GameplayCue.Skill.Shield");

		case EGamePlayTags0::Character_Buff_Shield:
		 return TEXT("Character.Buff.Shield");

		case EGamePlayTags0::Character_Skill_WirelessShooting:
		 return TEXT("Character.Skill.WirelessShooting");

	}

	return FString();
}

/*Convert a composite enumeration to the corresponding label.*/
void  NameToEGamePlayTags0s(const FName &InName,TArray<FName> &OutName)
{
	uint64 Tags = FCString::Strtoui64(*InName.ToString(),NULL,10);
	if(Tags & (int32)EGamePlayTags0::Character_Skill_Dodge)
	{
		OutName.AddUnique(TEXT("Character.Skill.Dodge"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_Sprint)
	{
		OutName.AddUnique(TEXT("Character.Skill.Sprint"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_State_Die)
	{
		OutName.AddUnique(TEXT("Character.State.Die"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_State_Hit)
	{
		OutName.AddUnique(TEXT("Character.State.Hit"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Buff_Damage)
	{
		OutName.AddUnique(TEXT("Character.Buff.Damage"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_WhirlwindBlow)
	{
		OutName.AddUnique(TEXT("Character.Skill.WhirlwindBlow"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_SprintWhirlwindBlow)
	{
		OutName.AddUnique(TEXT("Character.Skill.SprintWhirlwindBlow"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_EscapeToKill)
	{
		OutName.AddUnique(TEXT("Character.Skill.EscapeToKill"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_StreamBlade)
	{
		OutName.AddUnique(TEXT("Character.Skill.StreamBlade"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Cooldown_Skill)
	{
		OutName.AddUnique(TEXT("Character.Cooldown.Skill"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_Spike)
	{
		OutName.AddUnique(TEXT("Character.Skill.Spike"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Buff_Recovery_Health)
	{
		OutName.AddUnique(TEXT("Character.Buff.Recovery.Health"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Buff_Recovery_Mana)
	{
		OutName.AddUnique(TEXT("Character.Buff.Recovery.Mana"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Buff_Recovery_Stamina)
	{
		OutName.AddUnique(TEXT("Character.Buff.Recovery.Stamina"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_PickFly)
	{
		OutName.AddUnique(TEXT("Character.Skill.PickFly"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Attack_ComboLinkage_Air)
	{
		OutName.AddUnique(TEXT("Character.Attack.ComboLinkage.Air"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Attack_ComboLinkage_Ground)
	{
		OutName.AddUnique(TEXT("Character.Attack.ComboLinkage.Ground"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_SprintPickFly)
	{
		OutName.AddUnique(TEXT("Character.Skill.SprintPickFly"));
	}
	if(Tags & (int32)EGamePlayTags0::GameplayCue_Hit)
	{
		OutName.AddUnique(TEXT("GameplayCue.Hit"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_AVolley)
	{
		OutName.AddUnique(TEXT("Character.Skill.AVolley"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Attack_ComboLinkage_Ground_Type2)
	{
		OutName.AddUnique(TEXT("Character.Attack.ComboLinkage.Ground.Type2"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_FlameCutDown)
	{
		OutName.AddUnique(TEXT("Character.Skill.FlameCutDown"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_WhirlwindProvoked)
	{
		OutName.AddUnique(TEXT("Character.Skill.WhirlwindProvoked"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Buff_SuperArmor)
	{
		OutName.AddUnique(TEXT("Character.Buff.SuperArmor"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_State_Stun)
	{
		OutName.AddUnique(TEXT("Character.State.Stun"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_Block)
	{
		OutName.AddUnique(TEXT("Character.Skill.Block"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_Block_Type1)
	{
		OutName.AddUnique(TEXT("Character.Skill.Block.Type1"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_ShieldAtk)
	{
		OutName.AddUnique(TEXT("Character.Skill.ShieldAtk"));
	}
	if(Tags & (int32)EGamePlayTags0::GameplayCue_Skill_Shield)
	{
		OutName.AddUnique(TEXT("GameplayCue.Skill.Shield"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Buff_Shield)
	{
		OutName.AddUnique(TEXT("Character.Buff.Shield"));
	}
	if(Tags & (int32)EGamePlayTags0::Character_Skill_WirelessShooting)
	{
		OutName.AddUnique(TEXT("Character.Skill.WirelessShooting"));
	}
}

/*Convert multiple enumerations into one label for storage.*/
FName  EGamePlayTags0sToName(const TArray<FName> &InName)
{
	uint64 Tags = 0llu;
	for(auto &Tmp : InName)
	{
		if(Tmp == FName(TEXT("Character.Skill.Dodge")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_Dodge;
		}else
		if(Tmp == FName(TEXT("Character.Skill.Sprint")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_Sprint;
		}else
		if(Tmp == FName(TEXT("Character.State.Die")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_State_Die;
		}else
		if(Tmp == FName(TEXT("Character.State.Hit")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_State_Hit;
		}else
		if(Tmp == FName(TEXT("Character.Buff.Damage")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Buff_Damage;
		}else
		if(Tmp == FName(TEXT("Character.Skill.WhirlwindBlow")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_WhirlwindBlow;
		}else
		if(Tmp == FName(TEXT("Character.Skill.SprintWhirlwindBlow")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_SprintWhirlwindBlow;
		}else
		if(Tmp == FName(TEXT("Character.Skill.EscapeToKill")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_EscapeToKill;
		}else
		if(Tmp == FName(TEXT("Character.Skill.StreamBlade")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_StreamBlade;
		}else
		if(Tmp == FName(TEXT("Character.Cooldown.Skill")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Cooldown_Skill;
		}else
		if(Tmp == FName(TEXT("Character.Skill.Spike")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_Spike;
		}else
		if(Tmp == FName(TEXT("Character.Buff.Recovery.Health")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Buff_Recovery_Health;
		}else
		if(Tmp == FName(TEXT("Character.Buff.Recovery.Mana")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Buff_Recovery_Mana;
		}else
		if(Tmp == FName(TEXT("Character.Buff.Recovery.Stamina")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Buff_Recovery_Stamina;
		}else
		if(Tmp == FName(TEXT("Character.Skill.PickFly")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_PickFly;
		}else
		if(Tmp == FName(TEXT("Character.Attack.ComboLinkage.Air")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Attack_ComboLinkage_Air;
		}else
		if(Tmp == FName(TEXT("Character.Attack.ComboLinkage.Ground")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Attack_ComboLinkage_Ground;
		}else
		if(Tmp == FName(TEXT("Character.Skill.SprintPickFly")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_SprintPickFly;
		}else
		if(Tmp == FName(TEXT("GameplayCue.Hit")))
		{
			Tags |= (uint64)EGamePlayTags0::GameplayCue_Hit;
		}else
		if(Tmp == FName(TEXT("Character.Skill.AVolley")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_AVolley;
		}else
		if(Tmp == FName(TEXT("Character.Attack.ComboLinkage.Ground.Type2")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Attack_ComboLinkage_Ground_Type2;
		}else
		if(Tmp == FName(TEXT("Character.Skill.FlameCutDown")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_FlameCutDown;
		}else
		if(Tmp == FName(TEXT("Character.Skill.WhirlwindProvoked")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_WhirlwindProvoked;
		}else
		if(Tmp == FName(TEXT("Character.Buff.SuperArmor")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Buff_SuperArmor;
		}else
		if(Tmp == FName(TEXT("Character.State.Stun")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_State_Stun;
		}else
		if(Tmp == FName(TEXT("Character.Skill.Block")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_Block;
		}else
		if(Tmp == FName(TEXT("Character.Skill.Block.Type1")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_Block_Type1;
		}else
		if(Tmp == FName(TEXT("Character.Skill.ShieldAtk")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_ShieldAtk;
		}else
		if(Tmp == FName(TEXT("GameplayCue.Skill.Shield")))
		{
			Tags |= (uint64)EGamePlayTags0::GameplayCue_Skill_Shield;
		}else
		if(Tmp == FName(TEXT("Character.Buff.Shield")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Buff_Shield;
		}else
		if(Tmp == FName(TEXT("Character.Skill.WirelessShooting")))
		{
			Tags |= (uint64)EGamePlayTags0::Character_Skill_WirelessShooting;
		}
	}

	return *FString::Printf(TEXT("%llu"),Tags);
}

//////////////////////////Main/////////////////////////////
/*Convert BitAndOrder to bytes and TagName*/
bool SingleBitAndOrderToTagName(const int32 InEnumIndex, const uint64 EnumValue,FName &TagName)
{
	switch (InEnumIndex)
	{
	case 0:
		TagName = *EGamePlayTags0ToName((EGamePlayTags0)EnumValue);
		return true;
	}

	return false;
}

/*Convert FName to bytes and order*/
bool SingleTagNameToBitAndOrder(const FName& TageName, int32& InEnumIndex, uint64& EnumValue)
{
	int32 InOrder = GetGamePlayTagOrderIndex(TageName);
	if (InOrder != INDEX_NONE)
	{
		InEnumIndex = GetOrderMatchingEnumIndex(InOrder) - 1;
		switch (InEnumIndex)
		{
		case 0:
			EnumValue = (uint64)NameToEGamePlayTags0(TageName);
			return true;
		}
	}

	return true;
}

/*Convert the server's sequence to a label.*/
void  AnalysisArrayNameToGamePlayTags(const TArray<FName> &InNames,TArray<FName> &OutNames)
{
	for(auto &Tmp : InNames)
	{
		NameToEGamePlayTags0s(Tmp,OutNames);
	}
}

/*Convert multiple tags to a sequence stored by the server.*/
void  AnalysisGamePlayTagsToArrayName(const TArray<FName> &InNames,TArray<FName> &OutNames)
{
	FName TagName = EGamePlayTags0sToName(InNames);
	if(TagName != NAME_None)
	{
		OutNames.AddUnique(TagName);
	}
}
