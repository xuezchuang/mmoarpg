//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleComboType.generated.h"

class UAnimMontage;

//选自塔防课程
UENUM(BlueprintType)
enum class EHitCollisionType :uint8
{
	HITCOLLISIONTYPE_SHORT_RANGE_ATTACK UMETA(DisplayName = "Short range attack"),		//近程攻击

	HITCOLLISIONTYPE_DIRECT_LINE 		UMETA(DisplayName = "Direct Line"),				//无障碍直线攻击
	HITCOLLISIONTYPE_LINE				UMETA(DisplayName = "Line"),					//非跟踪类型，类似手枪子弹；
	HITCOLLISIONTYPE_TRACK_LINE			UMETA(DisplayName = "Track Line"),				//跟踪类型
	HITCOLLISIONTYPE_RANGE_LINE			UMETA(DisplayName = "Range Line"),				//范围伤害，丢手雷；
	HITCOLLISIONTYPE_RANGE				UMETA(DisplayName = "Range"),					//范围伤害，类似自爆；
	HITCOLLISIONTYPE_CHAIN				UMETA(DisplayName = "Chain"),					//链条类型，持续伤害类型;
};

USTRUCT(BlueprintType)
struct SIMPLECOMBAT_API FContinuousReleaseSpell
{
	GENERATED_USTRUCT_BODY()

	FContinuousReleaseSpell();

	UPROPERTY(EditDefaultsOnly, Category = ContinuousReleaseSpell)
	int32 ContinuousReleaseSpellIndex;

	UPROPERTY(EditDefaultsOnly, Category = ContinuousReleaseSpell)
	UAnimMontage* AnimMontage;

	class ISimpleCombatInterface* Character;

	UPROPERTY()
	UObject* BuffPtr;

	void Reset();
	void Press();
	void Released();
};

USTRUCT(BlueprintType)
struct SIMPLECOMBAT_API FSimpleComboCheck
{
	GENERATED_USTRUCT_BODY()

	FSimpleComboCheck();

	UPROPERTY(EditDefaultsOnly, Category = ComboAttack)
	int32 ComboIndex;

	UPROPERTY(EditDefaultsOnly, Category = ComboAttack)
	bool bLongPress;

	UPROPERTY(EditDefaultsOnly, Category = ComboAttack)
	bool bShortPress;

	class ISimpleCombatInterface* Character;

	UPROPERTY()
	int32 MaxIndex;

	UPROPERTY()
	FName ComboKey;

	void UpdateComboIndex();

	void Press();

	void Released();

	void Reset();

	FSimpleDelegate PressDelegate;
	FSimpleDelegate ReleasedDelegate;
	FSimpleDelegate ResetDelegate;
	FSimpleDelegate UpdateDelegate;
};

//判断是否相等
inline bool operator==(const FSimpleComboCheck& L, const FSimpleComboCheck& R)
{
	return L.ComboKey == R.ComboKey;
}

namespace SimpleComboType
{
	FVector GetCurrentCharacterDirection(ACharacter* InCharacter, const FVector& InDirectionForce);
}