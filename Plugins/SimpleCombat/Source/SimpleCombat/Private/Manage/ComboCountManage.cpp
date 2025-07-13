//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "Manage/ComboCountManage.h"
#include "UMG/UI_ComboCount.h"

FComboCountManage* FComboCountManage::Instance = NULL;

FComboCountManage::FComboCountManage()
	: ComboNumber(0)
	, TimeDelta(2.f)
	, MaxDeltaTime(2.f)
{

}

UUI_ComboCount* FComboCountManage::GetComboCount(UWorld *InWorld,TSubclassOf<UUI_ComboCount> InClass)
{
	if (InWorld && InClass)
	{
		if (UUI_ComboCount* MyComboCountPtr = CreateWidget<UUI_ComboCount>(InWorld, InClass))
		{
			MyComboCountPtr->AddToViewport(100);

			return MyComboCountPtr;
		}
	}

	return NULL;
}

void FComboCountManage::Tick(float DeltaTime)
{
	if (TimeDelta > 0.f)
	{
		TimeDelta -= DeltaTime;
		if (TimeDelta <=0.f)
		{
			TimeDelta = 0.f;
			ComboNumber = 0;
		}
	}
}

TStatId FComboCountManage::GetStatId() const
{
	return TStatId();
}

void FComboCountManage::Play(UWorld* InWorld, TSubclassOf<UUI_ComboCount> InClass)
{
	if (UUI_ComboCount *InComboCount = GetComboCount(InWorld, InClass))
	{
		TimeDelta = 2.f;
		ComboNumber++;

		InComboCount->SetComboText(ComboNumber);

		InComboCount->SetMaxDeltaTime(TimeDelta);

		//²¥·Å
		InComboCount->PlayAnim();
	}
}

FComboCountManage* FComboCountManage::Get()
{
	if (!Instance)
	{
		Instance = new FComboCountManage();
	}

	return Instance;
}

void FComboCountManage::Destroy()
{
	if (Instance)
	{
		delete Instance;
	}

	Instance = NULL;
}