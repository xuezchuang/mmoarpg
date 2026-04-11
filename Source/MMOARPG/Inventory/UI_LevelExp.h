#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_LevelExp.generated.h"

class UTextBlock;

UCLASS()
class UUI_LevelExp : public UUI_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "LevelExp")
	UTextBlock* Level;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "LevelExp")
	UTextBlock* CurrentExp;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "LevelExp")
	UTextBlock* RequiredExp;
};
