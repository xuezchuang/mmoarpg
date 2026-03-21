
#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "../Data/FItemData.h"
#include "UI_WindowSwitcher.h"
#include "UI_UpperUIBar.generated.h"


class UButton;
class UUI_CraftingWindow;
class UImage;
//class UUserWidget;



UCLASS()
class UUI_UpperUIBar : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UImage* IconTexture;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ActiveTextTab;

	//UPROPERTY(BlueprintReadOnly,meta = (BindWidget))
	//UPROPERTY(meta = (BindWidget))
	//UUserWidget* Widget_QuestLog;

	//UPROPERTY(meta = (BindWidget))
	//UUserWidget* WB_Inventory_Overall;

	////UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	//UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Crafting")
	//UPROPERTY(meta = (BindWidget))
	//UUI_CraftingWindow* WB_CraftingWindow;

	UPROPERTY(meta = (BindWidget))
	UUI_WindowSwitcher* WB_WindowSwitcher;

	//UPROPERTY(meta = (BindWidget))
	//UEditableTextBox* Account;

	//UPROPERTY(meta = (BindWidget))
	//UEditableTextBox* Password;

//	UPROPERTY(meta = (BindWidget))
//	UButton* SignInButton;
//
public:
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Close;

	UPROPERTY(EditAnywhere, Category="BP_Setting")
	E_UpSwitchType ActiveType;

	template<typename UserClass>
	void BindSwitcherDelegate(UserClass* Listener, void(UserClass::* Func)(E_UIType))
	{
		if (WB_WindowSwitcher)
		{
			WB_WindowSwitcher->WindowSwitchDelegate.BindUObject(Listener, Func);
		}
	}


public:
	virtual void NativeConstruct();

	virtual void NativeDestruct();

	void ShowWidget(E_UIType eType);
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
	UUserWidget* GetWidgetByType(E_UIType eType);
	UFUNCTION()
	void OnClosed();
//	TMap<FString, FString> OutMap;

	E_UIType m_eCurType;
	bool m_bVisibility = false;
protected:
	virtual void NativePreConstruct() override;

};
