//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleComboType.generated.h"

class UAnimMontage;

//ѡ�������γ�
UENUM(BlueprintType)
enum class EHitCollisionType :uint8
{
	HITCOLLISIONTYPE_SHORT_RANGE_ATTACK UMETA(DisplayName = "Short range attack"),		//���̹���

	HITCOLLISIONTYPE_DIRECT_LINE 		UMETA(DisplayName = "Direct Line"),				//���ϰ�ֱ�߹���
	HITCOLLISIONTYPE_LINE				UMETA(DisplayName = "Line"),					//�Ǹ������ͣ�������ǹ�ӵ���
	HITCOLLISIONTYPE_TRACK_LINE			UMETA(DisplayName = "Track Line"),				//��������
	HITCOLLISIONTYPE_RANGE_LINE			UMETA(DisplayName = "Range Line"),				//��Χ�˺��������ף�
	HITCOLLISIONTYPE_RANGE				UMETA(DisplayName = "Range"),					//��Χ�˺��������Ա���
	HITCOLLISIONTYPE_CHAIN				UMETA(DisplayName = "Chain"),					//�������ͣ������˺�����;
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

//�ж��Ƿ����
inline bool operator==(const FSimpleComboCheck& L, const FSimpleComboCheck& R)
{
	return L.ComboKey == R.ComboKey;
}

namespace SimpleComboType
{
	FVector GetCurrentCharacterDirection(ACharacter* InCharacter, const FVector& InDirectionForce);
}