#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_Clone_Boost.generated.h"

class UImage;

UCLASS()
class MMOARPG_API UUI_Clone_Boost : public UUI_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "CloneBoost")
	UImage* I_Icon;
};
