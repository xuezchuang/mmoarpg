//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"

class UUI_ComboCount;
class SIMPLECOMBAT_API FComboCountManage :public FTickableGameObject
{
public:
	FComboCountManage();

	static FComboCountManage* Get();
	static void Destroy();

	//Monitored by main thread 
	virtual void Tick(float DeltaTime);
	virtual TStatId GetStatId() const;

	UUI_ComboCount* GetComboCount(UWorld* InWorld, TSubclassOf<UUI_ComboCount> InClass);

	void Play(UWorld* InWorld, TSubclassOf<UUI_ComboCount> InClass);
protected:
	static FComboCountManage* Instance;

	int32 ComboNumber;
	float TimeDelta;
	float MaxDeltaTime;
};