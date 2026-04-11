#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_GoldWeightBar.generated.h"

class UTextBlock;

UCLASS()
class UUI_GoldWeightBar : public UUI_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "GoldWeightBar")
	UTextBlock* Gold;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "GoldWeightBar")
	UTextBlock* CurrentWeight;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "GoldWeightBar")
	UTextBlock* MaxWeight;
};
