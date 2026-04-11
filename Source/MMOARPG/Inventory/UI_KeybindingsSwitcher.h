#pragma once

#include "CoreMinimal.h"
#include "FItemData.h"
#include "UI_Base.h"
#include "UI_KeybindingsSwitcher.generated.h"

class UWidgetSwitcher;

UENUM(BlueprintType)
enum class E_KeybindingsInputType : uint8
{
	Keyboard,
	Gamepad,
};

USTRUCT(BlueprintType)
struct FKeybindingsSwitchRule
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keybindings")
	E_UIType UIType = E_UIType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keybindings")
	E_KeybindingsInputType InputType = E_KeybindingsInputType::Keyboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keybindings")
	FName WidgetName = NAME_None;
};

UCLASS()
class MMOARPG_API UUI_KeybindingsSwitcher : public UUI_Base
{
	GENERATED_BODY()

public:
	UUI_KeybindingsSwitcher(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UWidgetSwitcher* WidgetSwitcher_0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keybindings")
	TArray<FKeybindingsSwitchRule> SwitchRules;

	UFUNCTION(BlueprintCallable, Category = "Keybindings")
	bool SwitchByUIType(E_UIType InUIType, E_KeybindingsInputType InInputType);

	UFUNCTION(BlueprintCallable, Category = "Keybindings")
	bool SwitchByWidgetName(FName InWidgetName);

	UFUNCTION(BlueprintCallable, Category = "Keybindings")
	bool SwitchByWidgetIndex(int32 InWidgetIndex);

	UFUNCTION(BlueprintCallable, Category = "Keybindings")
	void SetCurrentInputType(E_KeybindingsInputType InInputType);

	UFUNCTION(BlueprintCallable, Category = "Keybindings")
	bool RefreshCurrentSwitch();

	UFUNCTION(BlueprintPure, Category = "Keybindings")
	E_UIType GetCurrentUIType() const { return CurrentUIType; }

	UFUNCTION(BlueprintPure, Category = "Keybindings")
	E_KeybindingsInputType GetCurrentInputType() const { return CurrentInputType; }

	UFUNCTION(BlueprintPure, Category = "Keybindings")
	FName GetCurrentWidgetName() const { return CurrentWidgetName; }

private:
	const FKeybindingsSwitchRule* FindSwitchRule(E_UIType InUIType, E_KeybindingsInputType InInputType) const;
	int32 FindWidgetIndexByName(FName InWidgetName) const;
	void BuildDefaultSwitchRules();

private:
	UPROPERTY(BlueprintReadOnly, Category = "Keybindings", meta = (AllowPrivateAccess = "true"))
	E_UIType CurrentUIType = E_UIType::None;

	UPROPERTY(BlueprintReadOnly, Category = "Keybindings", meta = (AllowPrivateAccess = "true"))
	E_KeybindingsInputType CurrentInputType = E_KeybindingsInputType::Keyboard;

	UPROPERTY(BlueprintReadOnly, Category = "Keybindings", meta = (AllowPrivateAccess = "true"))
	FName CurrentWidgetName = NAME_None;
};
