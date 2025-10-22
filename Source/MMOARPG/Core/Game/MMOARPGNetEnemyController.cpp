// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOARPGNetEnemyController.h"
#include "Character/MMOARPGMonster.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"


// AMMOARPGNetEnemyController.cpp

AMMOARPGNetEnemyController::AMMOARPGNetEnemyController()
{
    bAttachToPawn = true;
}

void AMMOARPGNetEnemyController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
    EnemyPawn = Cast<AMMOARPGMonster>(InPawn);

    if (ensure(EnemyPawn))
    {
        AnimInstance = EnemyPawn->GetMesh() ? EnemyPawn->GetMesh()->GetAnimInstance() : nullptr;
        // 默认速度交由服务器指定；这里不自设巡逻/追击速度
        HomeLocation = EnemyPawn->GetActorLocation(); // 兜底
    }
}

void AMMOARPGNetEnemyController::OnUnPossess()
{
    GetWorldTimerManager().ClearAllTimersForObject(this);
    CurrentAttackMontage = nullptr;
    TargetActor = nullptr;
    EnemyPawn = nullptr;
    AnimInstance = nullptr;
    Super::OnUnPossess();
}

// === 服务器指令实现 ===

void AMMOARPGNetEnemyController::Net_SetTarget(AActor* NewTarget)
{
    TargetActor = NewTarget;
}

void AMMOARPGNetEnemyController::Net_MoveTo(const FVector& Dest, float Speed, bool /*bChasing*/)
{
    if (!EnemyPawn) return;

    if (UCharacterMovementComponent* Move = EnemyPawn->GetCharacterMovement())
    {
        Move->MaxWalkSpeed = FMath::Max(0.f, Speed);
    }

    // 网络木偶：不判断“是否到达/是否在攻击范围”，这些由服务器决定
    MoveToLocation(Dest, /*AcceptanceRadius*/5.f, /*bStopOnOverlap*/false, /*bUsePathfinding*/true);
}

void AMMOARPGNetEnemyController::Net_FaceTo(const FVector& WorldPos)
{
    if (!EnemyPawn) return;
    const FRotator Rot = UKismetMathLibrary::FindLookAtRotation(EnemyPawn->GetActorLocation(), WorldPos);
    EnemyPawn->SetActorRotation(Rot);
}

void AMMOARPGNetEnemyController::Net_PlayAttack(int32 AttackIndex, float PlayRate, float ExpectedSecs)
{
    if (!EnemyPawn || !AnimInstance) return;

    if (!EnemyPawn->AttackAnimaions.IsValidIndex(AttackIndex)) return;

    CurrentAttackIndex = AttackIndex;
    CurrentAttackMontage = EnemyPawn->AttackAnimaions[AttackIndex];

    AnimInstance->Montage_Play(CurrentAttackMontage, PlayRate);

    // 仅用于客户端表现的“预期时长”回退到服务器新消息覆盖
    GetWorldTimerManager().ClearTimer(TimerHandle_AttackEnd);
    if (ExpectedSecs > 0.f)
    {
        GetWorldTimerManager().SetTimer(
            TimerHandle_AttackEnd,
            [this]()
            {
                // 动画到点了，但真正是否连段/再攻由服务器来新发PlayAttack or MoveTo
                CurrentAttackMontage = nullptr;
            },
            ExpectedSecs,
            false
        );
    }
}

void AMMOARPGNetEnemyController::Net_StopAttack()
{
    GetWorldTimerManager().ClearTimer(TimerHandle_AttackEnd);
    if (AnimInstance)
    {
        AnimInstance->Montage_Stop(0.0f);
    }
    CurrentAttackMontage = nullptr;
}

void AMMOARPGNetEnemyController::Net_ResetToHome(const FVector& InHome)
{
    HomeLocation = InHome;
    Net_StopAttack();

    if (EnemyPawn)
    {
        // 恢复血量显示由服务器再发 HealthUpdate；这里仅位移表现
        MoveToLocation(HomeLocation, /*AcceptanceRadius*/5.f, false, true);
    }
}

void AMMOARPGNetEnemyController::Net_HealthUpdate(int32 CurHP, int32 MaxHP)
{
    if (!EnemyPawn) return;

    EnemyPawn->TotalHealth = MaxHP;
    EnemyPawn->CurrentHealth = FMath::Clamp(CurHP, 0, MaxHP);
    EnemyPawn->UpdateHealthBar();
}

void AMMOARPGNetEnemyController::Net_Die()
{
    Net_StopAttack();
    if (EnemyPawn)
    {
        // 播死亡动画（建议服务器也明确下发具体Montage/时长）
        if (AnimInstance && EnemyPawn->DeathMontage) // 假设你有 DeathMontage
        {
            AnimInstance->Montage_Play(EnemyPawn->DeathMontage, 1.f);
        }
        // 禁用移动
        if (UCharacterMovementComponent* Move = EnemyPawn->GetCharacterMovement())
        {
            Move->DisableMovement();
        }
    }
}

void AMMOARPGNetEnemyController::Net_Despawn()
{
    Net_StopAttack();
    if (APawn* P = GetPawn())
    {
        P->Destroy();
    }
    Destroy();
}


