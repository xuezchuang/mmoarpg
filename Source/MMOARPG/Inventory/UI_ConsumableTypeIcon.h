#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "../Data/FItemData.h"
#include "UI_ConsumableTypeIcon.generated.h"

class UImage;

UCLASS()
class MMOARPG_API UUI_ConsumableTypeIcon : public UUI_Base
{
	GENERATED_BODY()

public:
	void SetByItemData(const FFS_ItemData* InItemData);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Inventory|ConsumableIcon")
	UImage* ConsumableTypeUsed;
};
