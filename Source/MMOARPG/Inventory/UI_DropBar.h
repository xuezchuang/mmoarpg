#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_DropBar.generated.h"

class UButton;
class UImage;

UCLASS()
class MMOARPG_API UUI_DropBar : public UUI_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "DropBar")
	UImage* bgimg;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "DropBar")
	UButton* Button_Drop;
};
