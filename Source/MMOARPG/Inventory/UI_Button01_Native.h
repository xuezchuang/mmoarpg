#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "../Data/FItemData.h"
#include "UI_WindowSwitcher.h"

#include "UI_Button01_Native.generated.h"

DECLARE_DELEGATE_OneParam(FWindowSwitch, E_UIType);

class UButton;
class UUI_NativeButton;
class UFontFace;
class UUI_Button01_Content;
class UUI_Button01_FX;
class USizeBox;
//class UUserWidget;

UCLASS()
class UUI_Button01_Native: public UUI_Base
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_NativeButton* WB_NativeButton;

	void NativePreConstruct() override;



public:
	//UPROPERTY(BlueprintReadOnly,meta = (BindWidget))
	UPROPERTY(meta = (BindWidget))
	UUI_Button01_FX* WB_Button01_FX;

private:
	//Normal Style
	//EditAnywhere VisibleInstanceOnly
	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnNormal")
	UFont* Font_Normal;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnNormal")
	UFontFace* TypeFace_Normal;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnNormal")
	int32 FontSize_Normal;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnNormal")
	FLinearColor FontColor_Normal;

	//Hovered Style
	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnHovered")
	UFont* Font_Hovered;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnHovered")
	UFontFace* TypeFace_Hovered;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnHovered")
	int32 FontSize_Hovered;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnHovered")
	FLinearColor FontColor_Hovered;

	//Clicked Style
	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnClicked")
	UFont* Font_Clicked;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnClicked")
	UFontFace* TypeFace_Clicked;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnClicked")
	int32 FontSize_Clicked;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnClicked")
	FLinearColor FontColor_Clicked;

	//Disabled Style
	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnDisabled")
	UFont* Font_Disable;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnDisabled")
	UFontFace* TypeFace_Disable;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnDisabled")
	int32 FontSize_Disable;

	UPROPERTY(EditInstanceOnly, Category = "Details|Style|OnDisabled")
	FLinearColor FontColor_Disable;

	UPROPERTY(EditInstanceOnly, Category = "Details|Type")
	E_UIType m_eCurType;

	UPROPERTY(meta = (BindWidget))
	USizeBox* SB_FX;
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USizeBox* SB_NativeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_Button01_Content* WB_Button01_Content;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Details|Content")
	FText ButtonText;

public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void ActiveButton(bool bActive, bool bPassivity = true);

	FWindowSwitch FunClicked;

private:
	UFUNCTION()
	void OnButtonClicked();
	UFUNCTION()
	void OnButtonHovered();
	UFUNCTION()
	void OnButtonUnhovered();

private:
	void SetNativeButtonClickAreaHeight();
	void SetFX_BaselineHeight();

	bool m_bDoOnce = true;
	bool m_bClicked = false;
};
