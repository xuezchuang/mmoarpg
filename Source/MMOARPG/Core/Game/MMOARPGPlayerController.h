// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MMOARPGPlayerController.generated.h"

class AMMOARPGCharacter;
class UUI_CharacterMenu;
class UUI_InGame;
class FSimpleChannel;
struct S_ROLE_PROP;
struct FFS_ItemData;

/** 当前输入上下文，决定哪些按键有效 */
UENUM(BlueprintType)
enum class EInputContext : uint8
{
	Game,       // 正常游戏：技能、系统快捷键
	VendorUI,   // 商人购买弹窗
	MenuUI,     // 角色菜单等（后续扩展）
};

UENUM(BlueprintType)
enum class ESystemHotkey : uint8
{
	SelectTarget,
	SelectSelf,
	SelectParty1,
	DeselectAll,
	ToggleMount,
	OpenMainMenu,
	ToggleCharacterMenu,
	Interaction,

	// 商人购买弹窗快捷键（可重绑定）
	VendorOpenDialog,    // 打开购买数量弹窗（默认 X）
	VendorBuy,           // 确认购买（默认 E，仅在 SplitStack 可见时生效）
	VendorCountDecrease,
	VendorCountIncrease,
	VendorLeave,
};

/** 商人弹窗快捷键广播 delegate */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnVendorHotkey, ESystemHotkey);

UCLASS()
class MMOARPG_API AMMOARPGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMMOARPGPlayerController();

	UFUNCTION(server, reliable)
	void ReplaceCharacter(int32 InCharacterID);

	void SetMainUserWidgetVisibility(ESlateVisibility InVisibility);
	void ToggleCharacterMenu();
	void CloseCharacterMenu();
	bool IsCharacterMenuOpen() const;

	void RebindSelectTargetKey(const FKey& NewKey);

	/** 商人弹窗快捷键 delegate，UI_SplitStack / VendorStorageInventory 订阅 */
	FOnVendorHotkey OnVendorHotkey;

	/** 切换输入上下文（开关商店、菜单时调用） */
	void SetInputContext(EInputContext NewContext);
	EInputContext GetInputContext() const { return CurrentInputContext; }

	/**
	 * 供 widget NativeOnKeyDown 调用：在 VendorUI 上下文中查找并执行对应 vendor 热键。
	 * UIOnly 模式下 InputComponent 不触发，通过此方法路由。
	 * @return true 表示已处理，false 表示未匹配。
	 */
	bool TryHandleVendorKey(FKey Key);

private:
	EInputContext CurrentInputContext = EInputContext::Game;

	/** 判断某个 hotkey 是否属于 VendorUI 上下文 */
	static bool IsVendorHotkey(ESystemHotkey Hotkey);

public:

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetupInputComponent() override;

	void OnAnyKeyPressed(FKey Key);
	void OnAnyKeyReleased(FKey Key);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUI_InGame> MainUserWidgetClass;

	void InitSkillSlots();
	void InitHotkeys();

	void CastSkillInSlot(int32 SlotIndex);
	void HandleSystemHotkey(ESystemHotkey Action);

	UPROPERTY()
	TArray<int32> SkillBar;

	UPROPERTY()
	TMap<FKey, int32> HotkeyToSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotkey")
	TMap<ESystemHotkey, FKey> HotkeyMap;

	UPROPERTY(BlueprintReadOnly)
	UUI_InGame* MainUserWidget = nullptr;

	UPROPERTY(BlueprintReadOnly)
	UUI_CharacterMenu* CharacterMenuWidget = nullptr;

	void OnSelectTarget();
	void RegisterInventoryHandlers();
	void UnregisterInventoryHandlers();
	void ScheduleInitialInventorySync();
	void StopInitialInventorySync();
	void TryInitialInventorySync();
	bool RequestInventorySync();
	void RecvInventoryProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);
	bool ApplyInventoryQuery(FSimpleChannel* Channel);
	bool BuildBagItemFromProp(const S_ROLE_PROP& InProp, FFS_ItemData& OutItem) const;
	void RefreshInventoryUI() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	AActor* CurrentSelectedTarget = nullptr;

	void SetCurrentTarget(AActor* NewTarget);
	void Interaction();
	UUI_CharacterMenu* GetOrCreateCharacterMenuWidget();
	AActor* FindBestEnemyTarget();
	AActor* FindBestFriendlyTarget();

	bool IsActorSelectable(AActor* Candidate, float MaxDistance, float MaxHalfAngleDeg) const;
	float CalcTargetScore(AActor* Candidate, const FVector& ViewLoc, const FVector& ViewDir) const;

	bool bInventoryHandlersRegistered = false;
	bool bInitialInventorySyncCompleted = false;
	bool bInitialInventorySyncRequested = false;
	FTimerHandle InitialInventorySyncTimerHandle;
};
