#pragma once

#include "CoreMinimal.h"
#include "../Core/UI_MainBase.h"
#include "FCharacterMenuData.h"
#include "UI_CharacterMenu.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UUI_WindowSwitcher;
class UWidgetSwitcher;

UCLASS(Abstract, Blueprintable)
class MMOARPG_API UUI_CharacterMenu : public UUI_MainBase
{
	GENERATED_BODY()

public:
	UUI_CharacterMenu(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION(BlueprintCallable, Category = "CharacterMenu")
	void ApplyMenuConfig(const FCharacterMenuConfig& InConfig);

	UFUNCTION(BlueprintCallable, Category = "CharacterMenu")
	void OpenMenu();

	UFUNCTION(BlueprintCallable, Category = "CharacterMenu")
	void CloseMenu();

	UFUNCTION(BlueprintCallable, Category = "CharacterMenu")
	void ToggleMenu();

	UFUNCTION(BlueprintCallable, Category = "CharacterMenu")
	void SetActiveTab(E_UIType InTabType);

	UFUNCTION(BlueprintPure, Category = "CharacterMenu")
	bool IsMenuOpen() const { return bIsMenuOpen; }

	UFUNCTION(BlueprintPure, Category = "CharacterMenu")
	E_UIType GetActiveTab() const { return ActiveTab; }

	UFUNCTION(BlueprintPure, Category = "CharacterMenu")
	FCharacterMenuConfig GetMenuConfig() const { return MenuConfig; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterMenu")
	void BP_OnMenuOpened();

	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterMenu")
	void BP_OnMenuClosed();

	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterMenu")
	void BP_OnActiveTabChanged(E_UIType InTabType);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UUI_WindowSwitcher* WB_WindowSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UWidgetSwitcher* ContentSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UButton* Button_Close;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UImage* HeaderIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* ActiveTextTab;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMenu")
	int32 ViewportZOrder = 200;

private:
	const FCharacterMenuTabData* FindTabData(E_UIType InTabType) const;
	void ApplyInputMode(bool bMenuOpen);
	void HandleTabChanged(E_UIType InTabType);

	UFUNCTION()
	void HandleCloseButtonClicked();

private:
	UPROPERTY(BlueprintReadOnly, Category = "CharacterMenu", meta = (AllowPrivateAccess = "true"))
	FCharacterMenuConfig MenuConfig;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterMenu", meta = (AllowPrivateAccess = "true"))
	E_UIType ActiveTab = E_UIType::None;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterMenu", meta = (AllowPrivateAccess = "true"))
	bool bIsMenuOpen = false;
};
