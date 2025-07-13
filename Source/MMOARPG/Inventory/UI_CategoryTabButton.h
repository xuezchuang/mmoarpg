#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "Engine\Texture2D.h"
#include "UI_CraftingListSpacer.h"
#include "UI_WindowSwitcher.h"
#include "UI_CategoryTabButton.generated.h"

class UButton;
class UImage;
class UBorder;
class UTextBlock;
//class UUserWidget;

UCLASS()
class UUI_CategoryTabButton : public UUI_Base
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UISetting")
	TObjectPtr<UTexture2D> CategoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UISetting")
	FText Text;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Item;

	UPROPERTY(meta = (BindWidget))
	UImage* HoverPulse;

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FLinearColor Icon_Active_Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FLinearColor Icon_NActive_Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FLinearColor Icon_Hover_Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FLinearColor Border_Active_Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "UISetting")
	FLinearColor Border_NActive_Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(meta = (BindWidget))
	UImage* ActiveLine;

	UPROPERTY(meta = (BindWidget))
	UBorder* Border_background;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextGuide;

	UPROPERTY(EditInstanceOnly, Category = "UISetting")
	E_UIType m_eCurType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UISetting")
	bool m_bActive = false;
//protected:
public:
	UPROPERTY(meta = (BindWidget))
	UButton* CT_Button;

	void Active(bool bActive);

	FWindowSwitch FunClicked;

private:
	UFUNCTION()
	void OnButtonClicked();

	UFUNCTION()
	void OnHover();

	UFUNCTION()
	void OnUnHover();

public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

protected:
	void NativePreConstruct() override;

private:
	

};
