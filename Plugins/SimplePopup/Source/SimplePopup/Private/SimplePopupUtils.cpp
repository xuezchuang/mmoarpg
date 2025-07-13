// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "SimplePopupUtils.h"
#include "SimplePopupType.h"
#include "Core/UI_SimplePopup.h"


bool FPopupHandle::Sure()
{
	if (SimplePopup.IsValid())
	{
		SimplePopup->Sure();
		return true;
	}

	return false;
}

bool FPopupHandle::Cancel()
{
	if (SimplePopup.IsValid())
	{
		SimplePopup->Cancel();
		return true;
	}

	return false;
}

FPopupHandle SimplePopupUtils::CreatePopup(
	UWorld *InWorld,
	TSubclassOf<UUI_SimplePopup> PopupClass,
	const FText &Content,
	ESimplePopupType SimplePopupType,
	const FText& SureButtonNameText,
	const FText& CancelButtonNameText,
	float Life /*= 10.f*/, 
	float DelayDestroy,
	FSimpleDelegate SureDelegate /*= FSimpleDelegate()*/,
	FSimpleDelegate CancelDelegate /*= FSimpleDelegate()*/,
	bool bLockView)
{
	FPopupHandle Handle;
	if (InWorld && PopupClass)
	{
		if (UUI_SimplePopup *SimplePopupPtr = CreateWidget<UUI_SimplePopup>(InWorld, PopupClass))
		{
			SimplePopupPtr->AddToViewport(100);

			SimplePopupPtr->SimpleCancelDelegate = CancelDelegate;
			SimplePopupPtr->SimpleSureDelegate = SureDelegate;
			SimplePopupPtr->Delay = Life;
			SimplePopupPtr->DelayDestroy = DelayDestroy;
			SimplePopupPtr->SimplePopupType = SimplePopupType;
			SimplePopupPtr->SetText(Content);
			SimplePopupPtr->InitPopup(SureButtonNameText, CancelButtonNameText, bLockView);

			Handle.SetSimplePopup(SimplePopupPtr);
		}
	}

	return Handle;
}

void FPopupHandle::SetSimplePopup(UUI_SimplePopup *NewSimplePopup)
{
	SimplePopup = NewSimplePopup;
}