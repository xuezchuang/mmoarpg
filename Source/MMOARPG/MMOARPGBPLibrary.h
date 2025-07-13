#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MMOARPGBPLibrary.generated.h"

class UWidget;

UENUM()
enum class EMMOARPG_InputMode : uint8
{
	Input_UI,
	Input_Game,
};

UCLASS()
class UMMOARPGBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "SimpleNetChannelTesting", meta = (WorldContext = "WorldContextObject"))
	static void MMOARPG_InputMode(const UObject* WorldContextObject, EMMOARPG_InputMode Input, UWidget* pWidget = NULL);

	UFUNCTION(BlueprintCallable, Category = "MMOARPGBPLibrary", meta = (WorldContext = "WorldContextObject"))
	static void MMOARPG_Test(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "MMOARPGBPLibrary")
	static void MMOARPG_Test2();
};
