#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "Engine/Texture2D.h"
#include "UI_CategoryButton.generated.h"

class UButton;
class UImage;
class UBorder;

UCLASS()
class UUI_CategoryButton : public UUI_Base
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget))
	UImage* Image_92;

	UPROPERTY(meta = (BindWidget))
	UImage* Button_Preview_Icon;

	UPROPERTY(meta = (BindWidget))
	UBorder* Border_Weapons;

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_Weapons;

	/** 分类图标纹理 — EditAnywhere 使得父 Widget 设计器里可以对每个实例单独赋值 */
	UPROPERTY(EditAnywhere, Category = "BP_Setting")
	UTexture2D* Texture;

	void SetActive(bool bActive);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	virtual void NativePreConstruct() override;
};
