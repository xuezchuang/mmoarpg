//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "SimpleComboType.h"
#include "CombatInterface/SimpleCombatInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

FSimpleComboCheck::FSimpleComboCheck()
	:ComboIndex(INDEX_NONE)
	, bLongPress(false)
	, bShortPress(false)
	, Character(NULL)
	, MaxIndex(0)
{

}

void FSimpleComboCheck::UpdateComboIndex()
{
	check(MaxIndex > 0);

	ComboIndex++;
	if (ComboIndex > MaxIndex)
	{
		ComboIndex = 1;
	}

	UpdateDelegate.ExecuteIfBound();
}

void FSimpleComboCheck::Press()
{
	if (ComboIndex == INDEX_NONE)
	{
		PressDelegate.ExecuteIfBound();
		ComboIndex++;

		//提醒程序员这里有bug
		check(Character);

		if (Character)
		{
			if (Character->ComboAttack(ComboKey))
			{

			}
		}
	}

	bShortPress = true;
	bLongPress = true;
}

void FSimpleComboCheck::Released()
{
	bLongPress = false;

	ReleasedDelegate.ExecuteIfBound();
}

void FSimpleComboCheck::Reset()
{
	ComboIndex = INDEX_NONE;

	ResetDelegate.ExecuteIfBound();
}

FContinuousReleaseSpell::FContinuousReleaseSpell()
	:ContinuousReleaseSpellIndex(INDEX_NONE)
	,BuffPtr(NULL)
{

}

void FContinuousReleaseSpell::Press()
{
	if (ContinuousReleaseSpellIndex == INDEX_NONE)
	{
		ContinuousReleaseSpellIndex++;
	}
}

void FContinuousReleaseSpell::Released()
{
	ContinuousReleaseSpellIndex = 2;
}

void FContinuousReleaseSpell::Reset()
{
	ContinuousReleaseSpellIndex = INDEX_NONE;
}

namespace SimpleComboType
{
	FVector GetCurrentCharacterDirection(ACharacter* InCharacter,const FVector &InDirectionForce)
	{
		FVector V = FVector::ZeroVector;
		if (InCharacter)
		{
			if (InDirectionForce.X != 0.f)
			{
				V += InCharacter->GetActorForwardVector() * InDirectionForce.X;
			}

			if (InDirectionForce.Y != 0.f)
			{
				V += InCharacter->GetActorRightVector() * InDirectionForce.Y;
			}

			if (InDirectionForce.Z != 0.f)
			{
				V += InCharacter->GetActorUpVector() * InDirectionForce.Z;
			}
		}

		return V;
	}
}