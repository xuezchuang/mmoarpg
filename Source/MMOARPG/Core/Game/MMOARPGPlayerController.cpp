// Fill out your copyright notice in the Description page of Project Settings.
#include "MMOARPGPlayerController.h"
#include "Character/MMOARPGCharacter.h"
#include "Character/MMOARPGPlayerCharacter.h"
#include "Core/Common/MMOARPGGameInstance.h"
#include "Core/Common/MMOARPGNetSubsystem.h"
#include "MMOARPGGameState.h"
#include "MMOARPGPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Engine/EngineBaseTypes.h"
#include "Protocol/GameProtocol.h"
#include "Stream/SimpleIOStream.h"
#include "MMOARPG.h"
#include "TimerManager.h"
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
		if (Widget->IsMenuOpen())
		{
			RequestInventorySync();
		}
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

	RegisterInventoryHandlers();
	ScheduleInitialInventorySync();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}

void AMMOARPGPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopInitialInventorySync();
	UnregisterInventoryHandlers();
	Super::EndPlay(EndPlayReason);
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

void AMMOARPGPlayerController::RegisterInventoryHandlers()
{
	if (bInventoryHandlersRegistered)
	{
		return;
	}

	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return;
	}

	if (UMMOARPGNetSubsystem* NetSub = GI->GetSubsystem<UMMOARPGNetSubsystem>())
	{
		NetSub->RegisterUniqueHandlers(
			{
				SP_InventoryQuery
			},
			FProtocolHandler::CreateUObject(this, &AMMOARPGPlayerController::RecvInventoryProtocol));
		bInventoryHandlersRegistered = true;
	}
}

void AMMOARPGPlayerController::UnregisterInventoryHandlers()
{
	if (!bInventoryHandlersRegistered)
	{
		return;
	}

	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		if (UMMOARPGNetSubsystem* NetSub = GI->GetSubsystem<UMMOARPGNetSubsystem>())
		{
			NetSub->UnRegisterUniqueHandlers(
				{
					SP_InventoryQuery
				});
		}
	}

	bInventoryHandlersRegistered = false;
}

void AMMOARPGPlayerController::ScheduleInitialInventorySync()
{
	if (bInitialInventorySyncCompleted)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (!World->GetTimerManager().IsTimerActive(InitialInventorySyncTimerHandle))
		{
			World->GetTimerManager().SetTimer(
				InitialInventorySyncTimerHandle,
				this,
				&AMMOARPGPlayerController::TryInitialInventorySync,
				1.0f,
				true,
				0.2f);
			UE_LOG(MMOARPG, Display, TEXT("[InventorySync] Schedule initial inventory sync"));
		}
	}
}

void AMMOARPGPlayerController::StopInitialInventorySync()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InitialInventorySyncTimerHandle);
	}
}

void AMMOARPGPlayerController::TryInitialInventorySync()
{
	if (bInitialInventorySyncCompleted || bInitialInventorySyncRequested)
	{
		StopInitialInventorySync();
		return;
	}

	UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>();
	if (!GI || !GI->CanSendGameplayProtocols())
	{
		return;
	}

	if (RequestInventorySync())
	{
		bInitialInventorySyncRequested = true;
		StopInitialInventorySync();
		UE_LOG(MMOARPG, Display, TEXT("[InventorySync] Initial inventory sync request sent"));
	}
}

bool AMMOARPGPlayerController::RequestInventorySync()
{
	UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>();
	if (!GI || !GI->GetClient())
	{
		return false;
	}

	if (!GI->CanSendGameplayProtocols())
	{
		return false;
	}

	if (FSimpleChannel* Channel = GI->GetClient()->GetChannel())
	{
		FSimpleProtocols<SP_InventoryQuery>::Send(Channel);
		UE_LOG(MMOARPG, Display, TEXT("[InventorySync] Send SP_InventoryQuery"));
		return true;
	}

	return false;
}

void AMMOARPGPlayerController::RecvInventoryProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	if (!Channel)
	{
		return;
	}

	switch (ProtocolNumber)
	{
	case SP_InventoryQuery:
	{
		if (ApplyInventoryQuery(Channel))
		{
			RefreshInventoryUI();
		}
		break;
	}
	default:
		break;
	}
}

bool AMMOARPGPlayerController::ApplyInventoryQuery(FSimpleChannel* Channel)
{
	TArray<uint8> Buffer;
	if (!Channel->Receive(Buffer))
	{
		return false;
	}

	FSimpleIOStream Stream(Buffer);
	Stream.Seek(sizeof(FSimpleBunchHead));

	uint16 ChildCmd = 0;
	uint8 Num = 0;
	Stream >> ChildCmd;
	Stream >> Num;

	if (ChildCmd != 0)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[InventorySync] Recv SP_InventoryQuery failed [childcmd:%u]"), ChildCmd);
		return false;
	}

	AMMOARPGGameState* MMOARPGGameState = GetWorld() ? GetWorld()->GetGameState<AMMOARPGGameState>() : nullptr;
	AMMOARPGPlayerState* MMOARPGPlayerState = GetPlayerState<AMMOARPGPlayerState>();
	if (!MMOARPGGameState || !MMOARPGPlayerState)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[InventorySync] Missing GameState or PlayerState"));
		return false;
	}

	TArray<FFS_ItemData>& BagItems = MMOARPGPlayerState->GetBagItems();
	BagItems.Empty();
	BagItems.Reserve(Num);

	for (uint8 i = 0; i < Num; ++i)
	{
		uint8 BagPos = 0;
		S_ROLE_PROP Prop{};
		Prop.reset();

		Stream >> BagPos;
		Stream.Read(&Prop, Prop.sendSize());

		FFS_ItemData Item;
		if (BuildBagItemFromProp(Prop, Item))
		{
			BagItems.Add(MoveTemp(Item));
		}
		else
		{
			UE_LOG(MMOARPG, Warning, TEXT("[InventorySync] Ignore unknown bag item [bagpos:%u propid:%d count:%u type:%u]"),
				BagPos, Prop.base.id, Prop.base.count, Prop.base.type);
		}
	}

	UE_LOG(MMOARPG, Display, TEXT("[InventorySync] Applied full bag sync [num:%u cached:%d]"), Num, BagItems.Num());
	bInitialInventorySyncCompleted = true;
	StopInitialInventorySync();
	return true;
}

bool AMMOARPGPlayerController::BuildBagItemFromProp(const S_ROLE_PROP& InProp, FFS_ItemData& OutItem) const
{
	if (InProp.base.id <= 0 || InProp.base.count <= 0)
	{
		return false;
	}

	const AMMOARPGGameState* MMOARPGGameState = GetWorld() ? GetWorld()->GetGameState<AMMOARPGGameState>() : nullptr;
	if (!MMOARPGGameState)
	{
		return false;
	}

	const FFS_ItemData* Template = MMOARPGGameState->FindItemByIndex(InProp.base.id);
	if (!Template)
	{
		return false;
	}

	OutItem = *Template;
	OutItem.Index = InProp.base.id;
	OutItem.Stacks.Quantity = static_cast<int32>(InProp.base.count);
	OutItem.Stats.Value = static_cast<float>(InProp.base.money);
	return true;
}

void AMMOARPGPlayerController::RefreshInventoryUI() const
{
	if (CharacterMenuWidget && CharacterMenuWidget->IsMenuOpen())
	{
		CharacterMenuWidget->RefreshInventoryWidgets();
	}
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
