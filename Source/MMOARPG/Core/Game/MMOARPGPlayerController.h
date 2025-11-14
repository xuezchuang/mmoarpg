// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MMOARPGPlayerController.generated.h"

class AMMOARPGCharacter;
class UUI_InGame;

UENUM(BlueprintType)
enum class ESystemHotkey : uint8
{
    SelectTarget,       // 选中当前目标
    SelectSelf,         // 选中自己
    SelectParty1,       // 选中队友1
    DeselectAll,        // Esc：取消所有选中
    ToggleMount,        // 上/下马
    OpenMainMenu,       // 打开主菜单
};


/**
 * 
 */
UCLASS()
class MMOARPG_API AMMOARPGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMMOARPGPlayerController();

	UFUNCTION(server, reliable)
	void ReplaceCharacter(int32 InCharacterID);

	void RebindSelectTargetKey(const FKey& NewKey);
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void OnAnyKeyPressed(FKey Key);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUI_InGame> MainUserWidgetClass;

    void InitSkillSlots();        // 初始化 SkillBar
    void InitHotkeys();           // 初始化 Key 映射

	void CastSkillInSlot(int32 SlotIndex);
	void HandleSystemHotkey(ESystemHotkey Action);

	UPROPERTY()
	TArray<int32> SkillBar;

	UPROPERTY()
    TMap<FKey, int32> HotkeyToSlot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hotkey")
    TMap<ESystemHotkey, FKey> HotkeyMap;

	UPROPERTY(BlueprintReadOnly)
	class UUI_InGame* MainUserWidget;


    void OnSelectTarget();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	AActor* CurrentSelectedTarget = nullptr;

	// 切换选中目标（统一入口，负责通知组件/蓝图/UI）
	void SetCurrentTarget(AActor* NewTarget);

	AActor* FindBestEnemyTarget();
	AActor* FindBestFriendlyTarget();

	// 用来做可视范围和视野锥过滤
	bool IsActorSelectable(AActor* Candidate, float MaxDistance, float MaxHalfAngleDeg) const;
    float CalcTargetScore(AActor* Candidate, const FVector& ViewLoc, const FVector& ViewDir) const;
};
