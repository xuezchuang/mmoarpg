#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_LoadingScreen.generated.h"

class UImage;

UCLASS()
class MMOARPG_API UUI_LoadingScreen : public UUI_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "LoadingScreen")
	UImage* FadeScreen;

public:
	UFUNCTION(BlueprintCallable, Category = "LoadingScreen")
	void PlayLoadingScreenAnim(EUMGSequencePlayMode::Type PlayMode = EUMGSequencePlayMode::Forward);

	UFUNCTION(BlueprintCallable, Category = "LoadingScreen")
	float GetLoadingScreenDuration() const;
};
