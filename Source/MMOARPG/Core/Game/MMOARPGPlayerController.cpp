// Fill out your copyright notice in the Description page of Project Settings.
#include "MMOARPGPlayerController.h"
#include "Character/MMOARPGCharacter.h"
#include "Character/MMOARPGPlayerCharacter.h"
#include "MMOARPGGameState.h"
#include "MMOARPGPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Engine/EngineBaseTypes.h"
#include "../../MMOARPGBPLibrary.h"
#include "MetanoiaCombat/UI_InGame.h"
#include "UI/Game/UI_CharacterMenu.h"
#include "Component/SelectableComponent.h"
#include "MonsterWorldSubsystem.h"
#include "Character/MMOARPGMonster.h"
#include "NetPlay/BladeIINetPlayer.h"
#include "PlayerWorldSubsystem.h"


AMMOARPGPlayerController::AMMOARPGPlayerController()
{
	bShowMouseCursor = false;
	
}

void AMMOARPGPlayerController::SetMainUserWidgetVisibility(ESlateVisibility InVisibility)
{
	if (MainUserWidget)
	{
		MainUserWidget->SetVisibility(InVisibility);
	}
}

void AMMOARPGPlayerController::ToggleCharacterMenu()
{
	if (UUI_CharacterMenu* Widget = GetOrCreateCharacterMenuWidget())
	{
		Widget->ToggleMenu();
	}
}

void AMMOARPGPlayerController::CloseCharacterMenu()
{
	if (CharacterMenuWidget)
	{
		CharacterMenuWidget->CloseMenu();
	}
}

bool AMMOARPGPlayerController::IsCharacterMenuOpen() const
{
	return CharacterMenuWidget && CharacterMenuWidget->IsMenuOpen();
}

void AMMOARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
    {
        return;
    }

	InitSkillSlots();
	InitHotkeys();

    if (MainUserWidgetClass && !MainUserWidget)
    {
        MainUserWidget = CreateWidget<UUI_InGame>(this, MainUserWidgetClass);
        if (MainUserWidget)
        {
            MainUserWidget->AddToViewport();
        }
    }

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}

void AMMOARPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindKey(EKeys::AnyKey, IE_Pressed, this, &AMMOARPGPlayerController::OnAnyKeyPressed);
	InputComponent->BindKey(EKeys::AnyKey, IE_Released, this, &AMMOARPGPlayerController::OnAnyKeyReleased);
}

void AMMOARPGPlayerController::SetInputContext(EInputContext NewContext)
{
	CurrentInputContext = NewContext;
}

bool AMMOARPGPlayerController::IsVendorHotkey(ESystemHotkey Hotkey)
{
	return Hotkey == ESystemHotkey::VendorOpenDialog
		|| Hotkey == ESystemHotkey::VendorBuy
		|| Hotkey == ESystemHotkey::VendorCountDecrease
		|| Hotkey == ESystemHotkey::VendorCountIncrease
		|| Hotkey == ESystemHotkey::VendorLeave;
}

bool AMMOARPGPlayerController::TryHandleVendorKey(FKey Key)
{
	if (CurrentInputContext != EInputContext::VendorUI)
	{
		return false;
	}

	for (auto& Pair : HotkeyMap)
	{
		if (IsVendorHotkey(Pair.Key) && Pair.Value == Key)
		{
			UE_LOG(MMOARPG, Display, TEXT("[DBG-PC] TryHandleVendorKey Key=%s -> Hotkey=%d"),
				*Key.ToString(), static_cast<int32>(Pair.Key));
			HandleSystemHotkey(Pair.Key);
			return true;
		}
	}
	return false;
}

void AMMOARPGPlayerController::OnAnyKeyPressed(FKey Key)
{
	// [DBG] 临时日志 — 测试完删除
	UE_LOG(MMOARPG, Display, TEXT("[DBG-PC] OnAnyKeyPressed Key=%s Context=%d"),
		*Key.ToString(), static_cast<int32>(CurrentInputContext));

	switch (CurrentInputContext)
	{
	case EInputContext::VendorUI:
	{
		// 只响应 Vendor 快捷键，其余全屏蔽
		for (auto& Pair : HotkeyMap)
		{
			if (IsVendorHotkey(Pair.Key) && Pair.Value == Key)
			{
				// [DBG] 临时日志 — 测试完删除
				UE_LOG(MMOARPG, Display, TEXT("[DBG-PC] VendorUI context -> HandleSystemHotkey(%d)"),
					static_cast<int32>(Pair.Key));
				HandleSystemHotkey(Pair.Key);
				return;
			}
		}
		// [DBG] 临时日志 — 测试完删除
		UE_LOG(MMOARPG, Display, TEXT("[DBG-PC] VendorUI context: key %s not a vendor key, discarded"), *Key.ToString());
		return; // 非 Vendor 键在此上下文下丢弃
	}

	case EInputContext::Game:
	default:
	{
		// 技能栏
		if (int32* SlotIndex = HotkeyToSlot.Find(Key))
		{
			CastSkillInSlot(*SlotIndex);
			return;
		}
		// 系统快捷键（跳过 Vendor 专属键）
		for (auto& Pair : HotkeyMap)
		{
			if (!IsVendorHotkey(Pair.Key) && Pair.Value == Key)
			{
				HandleSystemHotkey(Pair.Key);
				return;
			}
		}
		break;
	}
	}
}

void AMMOARPGPlayerController::OnAnyKeyReleased(FKey Key)
{
	const FKey* InteractionKeyPtr = HotkeyMap.Find(ESystemHotkey::Interaction);

	if (!InteractionKeyPtr)
		return;

	if (Key == *InteractionKeyPtr)
	{
		if (AMMOARPGCharacterBase* Base = GetPawn<AMMOARPGCharacterBase>())
		{
			Base->StopInteractionHold();
		}
	}
}


void AMMOARPGPlayerController::ReplaceCharacter_Implementation(int32 InCharacterID)
{
}

void AMMOARPGPlayerController::RebindSelectTargetKey(const FKey& NewKey)
{
	HotkeyMap[ESystemHotkey::SelectTarget] = NewKey;
}

void AMMOARPGPlayerController::InitSkillSlots()
{
    SkillBar.SetNum(30);

    for (int32 i = 0; i < SkillBar.Num(); i++)
    {
        SkillBar[i] = -1;
    }
}

void AMMOARPGPlayerController::InitHotkeys()
{
    HotkeyToSlot.Empty();

    HotkeyToSlot.Add(EKeys::One, 0);
    HotkeyToSlot.Add(EKeys::Two, 1);
    HotkeyToSlot.Add(EKeys::Three, 2);
    HotkeyToSlot.Add(EKeys::Four, 3);
    HotkeyToSlot.Add(EKeys::Five, 4);
    HotkeyToSlot.Add(EKeys::Six, 5);
    HotkeyToSlot.Add(EKeys::Seven, 6);
    HotkeyToSlot.Add(EKeys::Eight, 7);
    HotkeyToSlot.Add(EKeys::Nine, 8);

	if (!HotkeyMap.Contains(ESystemHotkey::SelectTarget))
	{
		HotkeyMap.Add(ESystemHotkey::SelectTarget, EKeys::Tab);
	}
	if (!HotkeyMap.Contains(ESystemHotkey::DeselectAll))
	{
		HotkeyMap.Add(ESystemHotkey::DeselectAll, EKeys::Escape);
	}
	if (!HotkeyMap.Contains(ESystemHotkey::ToggleCharacterMenu))
	{
		HotkeyMap.Add(ESystemHotkey::ToggleCharacterMenu, EKeys::I);
	}
	if (!HotkeyMap.Contains(ESystemHotkey::Interaction))
	{
		HotkeyMap.Add(ESystemHotkey::Interaction, EKeys::F);
	}

	// 商人弹窗默认快捷键
	// X → 打开购买数量弹窗（VendorOpenDialog）
	if (!HotkeyMap.Contains(ESystemHotkey::VendorOpenDialog))
	{
		HotkeyMap.Add(ESystemHotkey::VendorOpenDialog, EKeys::X);
	}
	// E → 确认购买（VendorBuy，仅在 SplitStack 可见时生效）
	if (!HotkeyMap.Contains(ESystemHotkey::VendorBuy))
	{
		HotkeyMap.Add(ESystemHotkey::VendorBuy, EKeys::E);
	}
	if (!HotkeyMap.Contains(ESystemHotkey::VendorCountDecrease))
	{
		HotkeyMap.Add(ESystemHotkey::VendorCountDecrease, EKeys::A);
	}
	if (!HotkeyMap.Contains(ESystemHotkey::VendorCountIncrease))
	{
		HotkeyMap.Add(ESystemHotkey::VendorCountIncrease, EKeys::D);
	}
	if (!HotkeyMap.Contains(ESystemHotkey::VendorLeave))
	{
		HotkeyMap.Add(ESystemHotkey::VendorLeave, EKeys::Escape);
	}
}

void AMMOARPGPlayerController::CastSkillInSlot(int32 SlotIndex)
{

    if (!SkillBar.IsValidIndex(SlotIndex))
        return;

    int32 SkillId = SkillBar[SlotIndex];
    if (SkillId < 0)
        return;

    if (auto* Ch = Cast<ACharacter>(GetPawn()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cast Skill Slot: %d  SkillId: %d"), SlotIndex, SkillId);
    }
}

void AMMOARPGPlayerController::HandleSystemHotkey(ESystemHotkey Action)
{
	switch (Action)
	{
		case ESystemHotkey::SelectTarget:
		{
			OnSelectTarget();
			break;
		}
		case ESystemHotkey::DeselectAll:
		{
			SetCurrentTarget(NULL);
			break;
		}
		case ESystemHotkey::ToggleCharacterMenu:
		{
			ToggleCharacterMenu();
			break;
		}
		case ESystemHotkey::Interaction:
		{
			Interaction();
			break;
		}
		case ESystemHotkey::VendorOpenDialog:
		case ESystemHotkey::VendorBuy:
		case ESystemHotkey::VendorCountDecrease:
		case ESystemHotkey::VendorCountIncrease:
		case ESystemHotkey::VendorLeave:
		{
			OnVendorHotkey.Broadcast(Action);
			break;
		}
		default:
			break;
	}
}


void AMMOARPGPlayerController::OnSelectTarget()
{
	AActor* NewTarget = FindBestEnemyTarget();
	SetCurrentTarget(NewTarget);
}

void AMMOARPGPlayerController::SetCurrentTarget(AActor* NewTarget)
{
    if (CurrentSelectedTarget == NewTarget)
    {
        return;
    }

    if (CurrentSelectedTarget)
    {
        if (USelectableComponent* OldSelectable =
            CurrentSelectedTarget->FindComponentByClass<USelectableComponent>())
        {
            OldSelectable->OnSelectionEnd();
        }
    }

    CurrentSelectedTarget = NewTarget;
	
    if (CurrentSelectedTarget)
    {
		if (USelectableComponent* NewSelectable = CurrentSelectedTarget->FindComponentByClass<USelectableComponent>())
        {
            NewSelectable->OnSelected();
			if (AMMOARPGMonster* TargetMonster = Cast<AMMOARPGMonster>(NewSelectable->GetOwner()))
			{
				MainUserWidget->TargetMonster = TargetMonster;
			}
			else
			{
				MainUserWidget->TargetMonster = nullptr;
			}

        }
    }
	else
	{
		MainUserWidget->TargetMonster = nullptr;
	}
	MainUserWidget->UpdateState();
}

void AMMOARPGPlayerController::Interaction()
{
	if (AMMOARPGCharacterBase* MMOARPGBase = GetPawn<AMMOARPGCharacterBase>())
	{
		MMOARPGBase->Interaction();
	}
}

UUI_CharacterMenu* AMMOARPGPlayerController::GetOrCreateCharacterMenuWidget()
{
	if (CharacterMenuWidget)
	{
		return CharacterMenuWidget;
	}

	AMMOARPGGameState* MMOARPGGameState = GetWorld() ? GetWorld()->GetGameState<AMMOARPGGameState>() : nullptr;
	if (!MMOARPGGameState)
	{
		return nullptr;
	}

	TSubclassOf<UUI_CharacterMenu> WidgetClass = MMOARPGGameState->GetCharacterMenuWidgetClass();
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterMenuWidgetClass is not configured on GameState."));
		return nullptr;
	}

	CharacterMenuWidget = CreateWidget<UUI_CharacterMenu>(this, WidgetClass);
	if (CharacterMenuWidget)
	{
		CharacterMenuWidget->ApplyMenuConfig(MMOARPGGameState->GetCharacterMenuConfig());
	}

	return CharacterMenuWidget;
}

bool AMMOARPGPlayerController::IsActorSelectable(AActor* Candidate, float MaxDistance, float MaxHalfAngleDeg) const
{
	if (!IsValid(Candidate))
    {
        return false;
    }

    APawn* MyPawn = GetPawn();
    if (!MyPawn)
    {
        return false;
    }

    FVector ViewLoc;
    FRotator ViewRot;
    GetPlayerViewPoint(ViewLoc, ViewRot);
    const FVector ViewDir = ViewRot.Vector();

    const FVector TargetLoc = Candidate->GetActorLocation();
    const FVector ToTarget  = TargetLoc - ViewLoc;
    const float Distance    = ToTarget.Size();

    if (Distance > MaxDistance)
    {
        return false;
    }

    const FVector DirNorm = ToTarget.GetSafeNormal();
    const float  Dot      = FVector::DotProduct(ViewDir, DirNorm);
    const float  CosHalf  = FMath::Cos(FMath::DegreesToRadians(MaxHalfAngleDeg));
    if (Dot < CosHalf)
    {
        return false;
    }

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(SelectTargetLOS), false, MyPawn);
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        ViewLoc,
        TargetLoc,
        ECC_Visibility,
        Params
    );

    if (bHit && Hit.GetActor() != Candidate)
    {
        return false;
    }

    return true;
}

float AMMOARPGPlayerController::CalcTargetScore(AActor* Candidate, const FVector& ViewLoc, const FVector& ViewDir) const
{
    const FVector TargetLoc = Candidate->GetActorLocation();
    const FVector ToTarget  = TargetLoc - ViewLoc;
    const float Distance    = ToTarget.Size();

    if (Distance <= KINDA_SMALL_NUMBER)
    {
        return 1e6f;
    }

    const FVector DirNorm = ToTarget.GetSafeNormal();
    const float Dot       = FVector::DotProduct(ViewDir, DirNorm);

    return Dot * 1000.f - Distance * 0.1f;
}

AActor* AMMOARPGPlayerController::FindBestEnemyTarget()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    UMonsterWorldSubsystem* MonsterSub = World->GetSubsystem<UMonsterWorldSubsystem>();
    if (!MonsterSub)
    {
        return nullptr;
    }

    TArray<AMMOARPGMonster*> Monsters;
    MonsterSub->GetAllAliveMonsters(Monsters);

    if (Monsters.Num() == 0)
    {
        return nullptr;
    }

    FVector ViewLoc;
    FRotator ViewRot;
    GetPlayerViewPoint(ViewLoc, ViewRot);
    const FVector ViewDir = ViewRot.Vector();

    const float MaxDistance     = 3000.f;  // 选怪最大距离，你可以调
    const float MaxHalfAngleDeg = 45.f;    // 视野半角（前方 90° 锥）

    AActor* BestTarget = nullptr;
    float   BestScore  = -FLT_MAX;

    for (AMMOARPGMonster* M : Monsters)
    {
        if (!IsActorSelectable(M, MaxDistance, MaxHalfAngleDeg))
        {
            continue;
        }

        const float Score = CalcTargetScore(M, ViewLoc, ViewDir);
        if (Score > BestScore)
        {
            BestScore  = Score;
            BestTarget = M;
        }
    }

    return BestTarget;
}

AActor* AMMOARPGPlayerController::FindBestFriendlyTarget()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    UPlayerWorldSubsystem* PlayerSub = World->GetSubsystem<UPlayerWorldSubsystem>();
    if (!PlayerSub)
    {
        return nullptr;
    }

    // 找本地的 ABladeIINetPlayer（你可以按自己项目修改）
    ABladeIINetPlayer* LocalPlayer = nullptr;
    if (APawn* MyPawn = GetPawn())
    {
        LocalPlayer = Cast<ABladeIINetPlayer>(MyPawn);
    }

    TArray<ABladeIINetPlayer*> Players;
    PlayerSub->GetAllOtherPlayers(Players);

    if (Players.Num() == 0)
    {
        return nullptr;
    }

    FVector ViewLoc;
    FRotator ViewRot;
    GetPlayerViewPoint(ViewLoc, ViewRot);
    const FVector ViewDir = ViewRot.Vector();

    const float MaxDistance     = 3000.f;
    const float MaxHalfAngleDeg = 60.f;    // 选队友可以稍微宽一点

    AActor* BestTarget = nullptr;
    float   BestScore  = -FLT_MAX;

    for (ABladeIINetPlayer* P : Players)
    {
        // 如果以后有阵营/队伍，先判断是否友方
        // if (!IsFriendly(LocalPlayer, P)) continue;

        if (!IsActorSelectable(P, MaxDistance, MaxHalfAngleDeg))
        {
            continue;
        }

        const float Score = CalcTargetScore(P, ViewLoc, ViewDir);
        if (Score > BestScore)
        {
            BestScore  = Score;
            BestTarget = P;
        }
    }

    return BestTarget;
}
