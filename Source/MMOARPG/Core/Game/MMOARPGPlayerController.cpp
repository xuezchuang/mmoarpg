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
#include "Component/SelectableComponent.h"


AMMOARPGPlayerController::AMMOARPGPlayerController()
{
	bShowMouseCursor = true;
	
}

void AMMOARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
    {
        return;
    }

	// 1. 初始化 SkillBar（30 个槽位）
	InitSkillSlots();

	// 2. 初始化默认快捷键
	InitHotkeys();

    if (MainUserWidgetClass && !MainUserWidget)
    {
        MainUserWidget = CreateWidget<UUI_InGame>(this, MainUserWidgetClass);
        if (MainUserWidget)
        {
            MainUserWidget->AddToViewport();

            // 如果你有热键初始化，放这里：
            //MainUserWidget->GenerateHotkeys(Keys, KeysPerRow);
        }
    }

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void AMMOARPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindKey(EKeys::AnyKey, IE_Pressed, this, &AMMOARPGPlayerController::OnAnyKeyPressed);
}

void AMMOARPGPlayerController::OnAnyKeyPressed(FKey Key)
{
    // 1. 是技能槽？
    if (int32* SlotIndex = HotkeyToSlot.Find(Key))
    {
        CastSkillInSlot(*SlotIndex);
        return;
    }

    // 2. 是系统热键？
    for (auto& Pair : HotkeyMap)
    {
        if (Pair.Value == Key)
        {
            HandleSystemHotkey(Pair.Key);
            return;
        }
    }
}

void AMMOARPGPlayerController::ReplaceCharacter_Implementation(int32 InCharacterID)
{
	//if (!GetPawn())
	//{
	//	return;
	//}

	//if (AMMOARPGCharacterBase* MMOARPGBase = GetPawn<AMMOARPGCharacterBase>())
	//{
	//	if (MMOARPGBase->GetID() == InCharacterID)
	//	{
	//		return;
	//	}
	//}

	//if (AMMOARPGGameState* InGameState = GetWorld()->GetGameState<AMMOARPGGameState>())
	//{
	//	if (FCharacterStyleTable* InStyleTable = InGameState->GetCharacterStyleTable(InCharacterID))
	//	{
	//		if (AMMOARPGCharacter* InNewCharacter = GetWorld()->SpawnActor<AMMOARPGCharacter>(
	//			InStyleTable->MMOARPGCharacterClass,
	//			GetPawn()->GetActorLocation(),
	//			GetPawn()->GetActorRotation()))
	//		{
	//			if (AMMOARPGPlayerState* InPlayerState = GetPlayerState<AMMOARPGPlayerState>())
	//			{
	//				//判断是不是主要玩家角色
	//				if (AMMOARPGPlayerCharacter* InPlayerCharacter = Cast<AMMOARPGPlayerCharacter>(InNewCharacter))
	//				{
	//					InPlayerCharacter->UpdateKneadingBoby(InPlayerState->GetCA());
	//					InPlayerCharacter->CallUpdateKneadingBobyOnClient(InPlayerState->GetCA());
	//				}

	//				APawn* InPawn = GetPawn();
	//				OnPossess(InNewCharacter);

	//				InPawn->Destroy(true);
	//			}
	//		}
	//	}
	//}
}

void AMMOARPGPlayerController::RebindSelectTargetKey(const FKey& NewKey)
{
	HotkeyMap[ESystemHotkey::SelectTarget] = NewKey;
}

void AMMOARPGPlayerController::InitSkillSlots()
{
    SkillBar.SetNum(30);

    // 这里为每个槽默认 SkillId = -1（空）
    for (int32 i = 0; i < SkillBar.Num(); i++)
    {
        SkillBar[i] = -1;
    }
}

void AMMOARPGPlayerController::InitHotkeys()
{
    HotkeyToSlot.Empty();

    // 默认 1～9 对应槽 0～8
    HotkeyToSlot.Add(EKeys::One, 0);
    HotkeyToSlot.Add(EKeys::Two, 1);
    HotkeyToSlot.Add(EKeys::Three, 2);
    HotkeyToSlot.Add(EKeys::Four, 3);
    HotkeyToSlot.Add(EKeys::Five, 4);
    HotkeyToSlot.Add(EKeys::Six, 5);
    HotkeyToSlot.Add(EKeys::Seven, 6);
    HotkeyToSlot.Add(EKeys::Eight, 7);
    HotkeyToSlot.Add(EKeys::Nine, 8);

    // 槽 9～29 默认空，不加映射
	HotkeyMap[ESystemHotkey::SelectTarget] = EKeys::Tab;
	HotkeyMap[ESystemHotkey::DeselectAll] = EKeys::Escape;
	//HotkeyMap[ESystemHotkey::SelectSelf] = EKeys::Tab;
	
	
}

void AMMOARPGPlayerController::CastSkillInSlot(int32 SlotIndex)
{

    if (!SkillBar.IsValidIndex(SlotIndex))
        return;

    int32 SkillId = SkillBar[SlotIndex];
    if (SkillId < 0)
        return; // 空槽

    // 调用角色执行技能
    if (auto* Ch = Cast<ACharacter>(GetPawn()))
    {
        // 你自己的函数，例如：
        // Ch->CastSkill(SkillId);

        UE_LOG(LogTemp, Warning, TEXT("Cast Skill Slot: %d  SkillId: %d"), SlotIndex, SkillId);
    }
}

void AMMOARPGPlayerController::HandleSystemHotkey(ESystemHotkey Action)
{
	switch (Action)
	{
	case ESystemHotkey::SelectTarget:
		OnSelectTarget();
		break;

    //case ESystemHotkey::SelectSelf:
    //    OnSelectSelf();
    //    break;

    //case ESystemHotkey::DeselectAll:
    //    OnDeselectAll();
    //    break;

    //case ESystemHotkey::ToggleMount:
    //    OnToggleMount();
    //    break;

    //case ESystemHotkey::OpenMainMenu:
    //    OnOpenMainMenu();
    //    break;

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

    // 旧目标取消选中
    if (CurrentSelectedTarget)
    {
        if (USelectableComponent* OldSelectable =
            CurrentSelectedTarget->FindComponentByClass<USelectableComponent>())
        {
            OldSelectable->OnSelectionEnd(this);   // 这里用你自己的函数名
        }
    }

    CurrentSelectedTarget = NewTarget;

    // 新目标被选中
    if (CurrentSelectedTarget)
    {
        if (USelectableComponent* NewSelectable =
            CurrentSelectedTarget->FindComponentByClass<USelectableComponent>())
        {
            NewSelectable->OnSelected(this);       // 这里用你自己的函数名
        }
    }

    // TODO：这里顺便通知 UI（比如广播委托，或者 GameInstanceSubsystem 的多播）
}

bool AMMOARPGPlayerController::IsActorSelectable(AActor* Candidate, float MaxDistance, float MaxHalfAngleDeg) const
{
    if (!Candidate || Candidate->IsPendingKill())
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

    // 距离限制
    if (Distance > MaxDistance)
    {
        return false;
    }

    // 视野锥限制：计算夹角
    const FVector DirNorm = ToTarget.GetSafeNormal();
    const float  Dot      = FVector::DotProduct(ViewDir, DirNorm);
    const float  CosHalf  = FMath::Cos(FMath::DegreesToRadians(MaxHalfAngleDeg));
    if (Dot < CosHalf)
    {
        return false;
    }

    // 视线遮挡（可见性判断）
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
        // 被别的东西挡住了
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

    // 一个简单的 score：角度 + 距离综合
    // Dot ∈ [-1,1]，越大越好。这里稍微压一点距离的影响。
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
    PlayerSub->GetAllOtherPlayers(Players, LocalPlayer);

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
