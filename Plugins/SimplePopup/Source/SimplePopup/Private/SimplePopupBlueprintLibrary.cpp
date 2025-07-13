// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "SimplePopupBlueprintLibrary.h"

FPopupHandle USimplePopupBlueprintLibrary::CreatePopup(
	UObject* WorldContextObject,
	TSubclassOf<UUI_SimplePopup> PopupClass,
	const FText &Content,
	ESimplePopupType SimplePopupType,
	FSimpleBlueprintDelegate SureDelegate,
	FSimpleBlueprintDelegate CancelDelegate,
	const FText& SureButtonNameText,
	const FText& CancelButtonNameText,
	float Life /*= 10.f*/,
	float DelayDestroy,
	bool bLockView )
{
	FPopupHandle Handle;
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World && PopupClass)
	{
		if (UUI_SimplePopup *SimplePopupPtr = CreateWidget<UUI_SimplePopup>(World, PopupClass))
		{
			SimplePopupPtr->AddToViewport(100);

			SimplePopupPtr->SimpleBlueprintCancelDelegate = CancelDelegate;
			SimplePopupPtr->SimpleBlueprintSureDelegate = SureDelegate;
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

bool USimplePopupBlueprintLibrary::PopupSure(FPopupHandle PopupHandle)
{
	return PopupHandle.Sure();
}

bool USimplePopupBlueprintLibrary::PopupCancel(FPopupHandle PopupHandle)
{
	return PopupHandle.Cancel();
}
