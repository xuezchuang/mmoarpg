#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "FItemData.h"
#include "UI_WindowSwitcher.generated.h"

//typedef TDelegate<void(E_UIType)> FWindowSwitch;
DECLARE_DELEGATE_OneParam(FWindowSwitch, E_UIType);

class UButton;
class UUI_CategoryTabButton;
class UWidgetSwitcher;
class UCanvasPanel;
class UImage;
class UTextBlock;

UENUM(BlueprintType)
enum class E_UpSwitchType : uint8
{
	None,
	MainTab,
	Vendor,
	Storage,
	LoadGame,
};

UCLASS()
class UUI_WindowSwitcher : public UUI_Base
{
	GENERATED_BODY()
public:
#pragma region Switcher
	UPROPERTY(BlueprintReadOnly,meta = (BindWidget))
	UWidgetSwitcher* Switcher;

	UPROPERTY(BlueprintReadOnly,meta = (BindWidget))
	UCanvasPanel* MainTab;

	UPROPERTY(BlueprintReadOnly,meta = (BindWidget))
	UCanvasPanel* Vendor;

	UPROPERTY(BlueprintReadOnly,meta = (BindWidget))
	UCanvasPanel* Storage;

	UPROPERTY(BlueprintReadOnly,meta = (BindWidget))
	UCanvasPanel* LoadGame;
#pragma endregion


#pragma region Storage

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_CategoryTabButton* WB_Category_Storage;

#pragma endregion


#pragma region MainTable
public:
	UPROPERTY(BlueprintReadOnly,meta = (BindWidget))
	UUI_CategoryTabButton* WB_Category_Inventory;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_CategoryTabButton* WB_Category_Quest;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_CategoryTabButton* WB_Category_Map;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_CategoryTabButton* WB_Category_Skill;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_CategoryTabButton* WB_Category_Rune;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_CategoryTabButton* WB_Category_Glossary;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_CategoryTabButton* WB_Category_Gallery;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_CategoryTabButton* WB_Category_Guide;


	//UPROPERTY(meta = (BindWidget))
	//UEditableTextBox* Account;
#pragma endregion

#pragma region Vendor

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_CategoryTabButton* WB_Category_Vendor_Buy;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUI_CategoryTabButton* WB_Category_Player_Sell;

#pragma endregion
public:
	E_UpSwitchType ActiveTabType;
	UImage* IconTexture;
	UTextBlock* ActiveTextTab;
public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	virtual void NativePreConstruct()override;

	void InitOutIcon(UImage* InIconTexture,UTextBlock* InActiveTextTab,E_UpSwitchType InActiveTabType);

	//void SetCallBackShowWidget(FWindowSwitch callback);
	//void SetCallBackShowWidget(FWindowSwitch&& callback);

//	bool EncryptionToLocal(const FString& InPaths);
//	bool DecryptionFromLocal(const FString& InPaths);
//public:
//	void ShowLoginWarnings();
//protected:
//	UFUNCTION()
//	void SignIn();
//
//	UFUNCTION()
//	void Register();
//
//	UFUNCTION()
//	void CallDropDownBox();
private:
	//TMap<FString, FString> OutMap;
	E_UIType m_eCurType = E_UIType::None;

	//UUI_CategoryTabButton* GetUIButtonByType(E_UIType eType);

public:
	UFUNCTION(BlueprintCallable, Category = "UISetting")
	UUI_CategoryTabButton* GetUIButtonByType(E_UIType eType);

	UFUNCTION(BlueprintCallable, Category = "UISetting")
	void ApplyType(E_UIType eType, bool bBroadcast = false);

	UFUNCTION(BlueprintPure, Category = "UISetting")
	E_UIType GetCurrentType() const { return m_eCurType; }
	



private:
	UFUNCTION()
	void SetNativeUIType(E_UIType eType);

	UFUNCTION(BlueprintCallable, Category = "UISetting")
	void SetType(E_UIType eType, bool bPassivity = false);
public:

	FWindowSwitch WindowSwitchDelegate;
};
