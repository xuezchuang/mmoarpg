// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "SimpleRuntimeGamePrintingBPLibrary.h"
#include "SimpleRuntimeGamePrinting.h"
#include "UMG/UI_GamePrintf.h"

USimpleRuntimeGamePrintingBPLibrary::USimpleRuntimeGamePrintingBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool USimpleRuntimeGamePrintingBPLibrary::K2_GamePrintText(
	UObject* WorldContextObject,
	TSubclassOf<UUI_GamePrintf> GamePrintfClass,
	const FText& InText,
	float InDieDelayTime)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return USimpleRuntimeGamePrintingBPLibrary::GamePrint(World, GamePrintfClass, InText, InDieDelayTime);
	}

	return false;
}

bool USimpleRuntimeGamePrintingBPLibrary::K2_GamePrint(
	UObject* WorldContextObject,
	TSubclassOf<UUI_GamePrintf> GamePrintfClass,
	const TArray<FSimplePrintSlot>& InSlot,
	float InDieDelayTime)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		SimpleRuntimeGamePrinting::FPrintSlot PrintSlot;
		for (auto& Tmp : InSlot)
		{
			PrintSlot.AddSlot()
			.Color(Tmp.Color)
			[
				Tmp.Content
			];

			if (!Tmp.ImgID.IsEmpty())
			{
				PrintSlot.AddSlot()
				.Image(Tmp.ImgID);
			}
		}

		return USimpleRuntimeGamePrintingBPLibrary::GamePrint(World, GamePrintfClass, PrintSlot, InDieDelayTime);
	}

	return false;
}

bool USimpleRuntimeGamePrintingBPLibrary::GamePrint(
	UWorld* InWorld,
	TSubclassOf<UUI_GamePrintf> GamePrintfClass,
	const SimpleRuntimeGamePrinting::FPrintSlot& InSlot,
	float InDieDelayTime)
{
	return GamePrint(InWorld,
		GamePrintfClass,
		FText::FromString(InSlot.ToString()),
		InDieDelayTime);
}

bool USimpleRuntimeGamePrintingBPLibrary::GamePrint(
	UWorld* InWorld,
	TSubclassOf<UUI_GamePrintf> GamePrintfClass,
	const FText& InSlot,
	float InDieDelayTime)
{
	if (InWorld && GamePrintfClass)
	{
		if (UUI_GamePrintf* GamePrintfPtr = CreateWidget<UUI_GamePrintf>(InWorld, GamePrintfClass))
		{
			GamePrintfPtr->AddToViewport(100);
			GamePrintfPtr->SetDieDelayTime(InDieDelayTime);

			GamePrintfPtr->SetText(InSlot);

			//开始构造
			GamePrintfPtr->WidgetConstruct();

			return true;
		}
	}

	return false;
}