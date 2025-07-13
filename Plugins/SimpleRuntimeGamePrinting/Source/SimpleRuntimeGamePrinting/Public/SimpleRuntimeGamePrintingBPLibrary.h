// Copyright (C) RenZhai.2021.All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimpleRuntimeGamePrintingType.h"
#include "SimpleRuntimeGamePrintingBPLibrary.generated.h"

class UWorld;
class UUI_GamePrintf;

UCLASS()
class SIMPLERUNTIMEGAMEPRINTING_API USimpleRuntimeGamePrintingBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GamePrintText", Category = "SRGP",WorldContext = WorldContextObject))
		static bool K2_GamePrintText(
		UObject* WorldContextObject,
		TSubclassOf<UUI_GamePrintf> GamePrintfClass,
		const FText& InText,
		float InDieDelayTime = 7.0f);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GamePrint", Category = "SRGP",WorldContext = WorldContextObject))
	static bool K2_GamePrint(
		UObject* WorldContextObject,
		TSubclassOf<UUI_GamePrintf> GamePrintfClass,
		const TArray<FSimplePrintSlot>& InSlot,
		float InDieDelayTime = 7.0f);

	static bool GamePrint(
		UWorld* InWorld,
		TSubclassOf<UUI_GamePrintf> GamePrintfClass,
		const SimpleRuntimeGamePrinting::FPrintSlot& InSlot,
		float InDieDelayTime = 7.0f);

	static bool GamePrint(
		UWorld* InWorld,
		TSubclassOf<UUI_GamePrintf> GamePrintfClass,
		const FText& InSlot,
		float InDieDelayTime = 7.0f);
};
