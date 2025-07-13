// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MMOARPGGameType.generated.h"

//UENUM(BlueprintType)
//enum class ECharacterActionState : uint8
//{
//	NORMAL_STATE	UMETA(DisplayName = "Normal"),
//	FIGHT_STATE		UMETA(DisplayName = "Fight"),
//	FLIGHT_STATE	UMETA(DisplayName = "Flight"),
//	CLIMB_STATE		UMETA(DisplayName = "Climb"),
//	SWIMMING_STATE	UMETA(DisplayName = "Swimming"),
//};

UENUM(BlueprintType)
enum class EDodgeFly : uint8
{
	DODGE_NONE		UMETA(DisplayName = "None"),
	DODGE_LEFT		UMETA(DisplayName = "Left"),
	DODGE_RIGHT		UMETA(DisplayName = "Right"),
};

struct FResetBool
{
	FResetBool()
		:bSet(false)
		,Time(0.f)
	{}

	void Tick(float DeltaTime)
	{
		if (Time > 0.f)
		{
			Time -= DeltaTime;
			if (Time <= 0.f)
			{
				Fun.ExecuteIfBound();
				Time = 0.f;
				bSet = false;
			}
		}
	}

	FResetBool &operator=(bool bNewSet)
	{
		bSet = bNewSet;
		return *this;
	}

	FResetBool& operator=(float InNewTime)
	{
		Time = InNewTime;
		return *this;
	}

	bool operator*()
	{
		return bSet;
	}

	explicit operator bool()
	{
		return bSet;
	}

	FSimpleDelegate Fun;

protected:
	bool bSet;
	float Time;
};