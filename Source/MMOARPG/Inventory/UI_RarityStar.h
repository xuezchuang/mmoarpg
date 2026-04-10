#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_RarityStar.generated.h"

class UImage;

/**
 * UI_RarityStar — 单颗稀有度星星 Widget
 * Blueprint 子类需包含名为 Image_51 的 Image 控件
 */
UCLASS()
class MMOARPG_API UUI_RarityStar : public UUI_Base
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget))
	UImage* Image_51;

public:
	/** 设置星星图标贴图 */
	void SetStarTexture(UTexture2D* InTexture);

	/** 显示 / 隐藏该星星 */
	void SetStarVisible(bool bVisible);
};
