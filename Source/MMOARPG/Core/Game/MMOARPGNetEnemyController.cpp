#include "MMOARPGNetEnemyController.h"
#include "Character/MMOARPGMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


AMMOARPGNetEnemyController::AMMOARPGNetEnemyController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true; // 需要 Tick 做插值
}


void AMMOARPGNetEnemyController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // 网络版不跑移动意图：只根据服务器状态设置 Transform
    if (auto* TempEnemyPawn = Cast<AMMOARPGMonster>(InPawn))
    {
        if (auto* Move = TempEnemyPawn->GetCharacterMovement())
        {
            Move->DisableMovement();         // 禁用 CharacterMovement 的意图驱动
            Move->bOrientRotationToMovement = false;
        }
        PrevVisualPos = TempEnemyPawn->GetActorLocation();
        bHasPrevVisualPos = true;
    }

    // 清空任何单机 AI 用的计时器（避免误触）
    GetWorldTimerManager().ClearAllTimersForObject(this);
}

void AMMOARPGNetEnemyController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!EnemyPawn) return;

    if (StateBuffer.Num() == 0)
    {
        // 没状态，就让速度缓到0，避免动画抖动
        NetSpeed = FMath::FInterpTo(NetSpeed, 0.f, DeltaSeconds, 5.f);
        return;
    }

    const double ClientRenderTime = ComputeClientRenderTime();

    FNetMonsterState A, B;
    if (!SampleStates(ClientRenderTime, A, B))
    {
        // 只有最近一帧可用：用最近帧缓动到位
        const FNetMonsterState& Last = StateBuffer.Last();

        FVector Target = Last.Pos;
        if (bAdjustZOnApply) AdjustZToGround(EnemyPawn, Target);

        if (FVector::DistSquared(Target, EnemyPawn->GetActorLocation()) > FMath::Square(SnapThreshold))
        {
            EnemyPawn->SetActorLocation(Target, false, nullptr, ETeleportType::TeleportPhysics);
            EnemyPawn->SetActorRotation(Last.Rot);

            NetAction = Last.Action;
            NetSpeed  = Last.Speed;
        }
        else
        {
            const FVector NewPos  = FMath::VInterpTo(EnemyPawn->GetActorLocation(), Target, DeltaSeconds, 12.f);
            const FRotator NewRot = FMath::RInterpTo(EnemyPawn->GetActorRotation(), Last.Rot, DeltaSeconds, 12.f);

            EnemyPawn->SetActorLocation(NewPos);
            EnemyPawn->SetActorRotation(NewRot);

            NetAction = Last.Action;
            NetSpeed  = (Last.Speed > 0.f)
                        ? Last.Speed
                        : (NewPos - PrevVisualPos).Size() / FMath::Max(DeltaSeconds, KINDA_SMALL_NUMBER);
        }
    }
    else
    {
        ApplyInterpolated(EnemyPawn, A, B, ClientRenderTime, DeltaSeconds);
    }

    PrevVisualPos = EnemyPawn->GetActorLocation();

    // 修剪过旧状态（保留最近 0.5s）
    const double Cut = ClientRenderTime - 0.5;
    int32 FirstUseful = 0;
    while (FirstUseful < StateBuffer.Num() && StateBuffer[FirstUseful].ServerTime < Cut) ++FirstUseful;
    if (FirstUseful > 0) StateBuffer.RemoveAt(0, FirstUseful, false);
}

// ================== 网络接口 ==================

void AMMOARPGNetEnemyController::QueueNetState(const FNetMonsterState& InState)
{
    InsertStateSorted(InState);
}

void AMMOARPGNetEnemyController::QueueNetStates(const TArray<FNetMonsterState>& States)
{
    for (const auto& S : States) InsertStateSorted(S);
}

void AMMOARPGNetEnemyController::ClearStateBuffer()
{
    StateBuffer.Reset();
}

void AMMOARPGNetEnemyController::SnapToState(const FNetMonsterState& InState, bool bAdjustZ)
{
    if (!EnemyPawn) return;

    FVector P = InState.Pos;
    if (bAdjustZ && bAdjustZOnApply) AdjustZToGround(EnemyPawn, P);

    EnemyPawn->SetActorLocation(P, false, nullptr, ETeleportType::TeleportPhysics);
    EnemyPawn->SetActorRotation(InState.Rot);

    NetAction = InState.Action;
    NetSpeed  = InState.Speed;

    PrevVisualPos     = P;
    bHasPrevVisualPos = true;

    StateBuffer.Reset();
    StateBuffer.Add(InState);
}

// ================== 内部：插值实现 ==================

double AMMOARPGNetEnemyController::ComputeClientRenderTime() const
{
    const UWorld* W = GetWorld();
    const double Now = W ? W->GetTimeSeconds() : 0.0;
    return Now - FMath::Max(InterpDelaySeconds, 0.f);
}

void AMMOARPGNetEnemyController::InsertStateSorted(const FNetMonsterState& InState)
{
    int32 InsertIdx = StateBuffer.Num();
    for (int32 i = StateBuffer.Num() - 1; i >= 0; --i)
    {
        if (InState.ServerTime >= StateBuffer[i].ServerTime)
        {
            InsertIdx = i + 1;
            break;
        }
    }
    StateBuffer.Insert(InState, InsertIdx);

    // 同时间戳去重：保留新
    for (int32 i = StateBuffer.Num() - 2; i >= 0; --i)
    {
        if (FMath::IsNearlyEqual(StateBuffer[i].ServerTime, StateBuffer[i + 1].ServerTime))
        {
            StateBuffer.RemoveAt(i, 1, false);
            break;
        }
    }

    if (StateBuffer.Num() > MaxBufferSize)
        StateBuffer.RemoveAt(0, StateBuffer.Num() - MaxBufferSize, false);
}

bool AMMOARPGNetEnemyController::SampleStates(double ClientRenderTime, FNetMonsterState& OutPrev, FNetMonsterState& OutNext) const
{
    const int32 N = StateBuffer.Num();
    if (N <= 1) return false;

    for (int32 i = 0; i < N - 1; ++i)
    {
        const auto& A = StateBuffer[i];
        const auto& B = StateBuffer[i + 1];
        if (A.ServerTime <= ClientRenderTime && ClientRenderTime <= B.ServerTime)
        {
            OutPrev = A;
            OutNext = B;
            return true;
        }
    }
    return false; // 让外层用最近帧
}

void AMMOARPGNetEnemyController::ApplyInterpolated(
    AMMOARPGMonster* tEnemyPawn,
    const FNetMonsterState& A,
    const FNetMonsterState& B,
    double ClientRenderTime,
    float DeltaSeconds)
{
	EnemyPawn = tEnemyPawn;
    if (!EnemyPawn || DeltaSeconds <= 0.f)
    {
        return;
    }

    // ---- 时间与插值因子 ----
    const double Den  = FMath::Max(B.ServerTime - A.ServerTime, KINDA_SMALL_NUMBER);
    float Alpha = static_cast<float>((ClientRenderTime - A.ServerTime) / Den);
    Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

    // ---- 目标状态（线性插值）----
    const FVector  TargetPos = FMath::Lerp(A.Pos,   B.Pos,   Alpha);
    const FRotator TargetRot = FMath::Lerp(A.Rot,   B.Rot,   Alpha);
    float          TargetSpd = FMath::Lerp(A.Speed, B.Speed, Alpha);
    const ENetMonsterAction TargetAct = (Alpha < 0.5f) ? A.Action : B.Action;

    // ---- 大跳变时直接“瞬移”对齐（防止橡皮筋）----
    const FVector CurPos = EnemyPawn->GetActorLocation();
    if (FVector::DistSquared(TargetPos, CurPos) > FMath::Square(SnapThreshold))
    {
        FVector SnapPos = TargetPos;
        if (bAdjustZOnApply)
        {
            AdjustZToGround(EnemyPawn, SnapPos);
        }

        EnemyPawn->SetActorLocation(SnapPos, /*bSweep*/false, /*OutHit*/nullptr, ETeleportType::TeleportPhysics);
        EnemyPawn->SetActorRotation(TargetRot);

        // 记录网络动作与速度
        NetAction = TargetAct;
        NetSpeed  = FMath::Max(0.f, TargetSpd);

        // 缓存可视位置（供下一帧“速度回退估计”使用）
        PrevVisualPos     = SnapPos;
        bHasPrevVisualPos = true;
        return;
    }

    // ---- 小偏差：做平滑插值 ----
    FVector  AimPos = TargetPos;
    if (bAdjustZOnApply)
    {
        AdjustZToGround(EnemyPawn, AimPos);
    }

    // 位置/旋转平滑（插值速率可按手感调整）
    constexpr float LocInterpSpeed = 14.f;
    constexpr float RotInterpSpeed = 14.f;

    const FVector  NewPos = FMath::VInterpTo(CurPos, AimPos, DeltaSeconds, LocInterpSpeed);
    const FRotator NewRot = FMath::RInterpTo(EnemyPawn->GetActorRotation(), TargetRot, DeltaSeconds, RotInterpSpeed);

    EnemyPawn->SetActorLocation(NewPos);
    EnemyPawn->SetActorRotation(NewRot);

    // ---- 速度回退估计：当网络给的速度不可靠/为零时，用屏幕位移估算 ----
    if ((TargetSpd <= 0.f || !FMath::IsFinite(TargetSpd)) && bHasPrevVisualPos)
    {
        const float Moved = (NewPos - PrevVisualPos).Size();
        // 防止异常：DeltaSeconds>0 已在开头判断
        TargetSpd = Moved / DeltaSeconds;  // cm/s
    }

    // 速度/动作收敛与钳制（可根据项目需要调整上限）
    constexpr float MaxReasonableSpeed = 2000.f; // cm/s 举例
    TargetSpd = FMath::Clamp(FMath::IsFinite(TargetSpd) ? TargetSpd : 0.f, 0.f, MaxReasonableSpeed);

    // 如果网络动作不可信，可以依据速度修正动作（可选）
    ENetMonsterAction FinalAct = TargetAct;
    //if (FinalAct == ENetMonsterAction::None)
    //{
    //    FinalAct = (TargetSpd > 5.f) ? ENetMonsterAction::Move : ENetMonsterAction::Idle;
    //}

    NetAction = FinalAct;
    NetSpeed  = TargetSpd;

    // ---- 更新可视位置缓存 ----
    PrevVisualPos     = NewPos;
    bHasPrevVisualPos = true;
}

void AMMOARPGNetEnemyController::AdjustZToGround(AMMOARPGMonster* tEnemyPawn, FVector& InOutPos) const
{
    if (!tEnemyPawn)
    {
        return;
    }

    UWorld* World = tEnemyPawn->GetWorld();
    if (!World)
    {
        return;
    }

    // 起点稍微高一点，避免从地面以下射线
    FVector Start = InOutPos + FVector(0.f, 0.f, 200.f);
    FVector End   = InOutPos - FVector(0.f, 0.f, 1000.f);

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(AdjustZToGround), false, tEnemyPawn);

    // 使用可行走表面（Visibility 通道）检测地面
    if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        // 落点的地面坐标
        InOutPos.Z = Hit.ImpactPoint.Z;

#if WITH_EDITOR
        // 可视化调试线（只在编辑器可见）
        DrawDebugLine(World, Start, Hit.ImpactPoint, FColor::Green, false, 1.0f, 0, 1.0f);
        DrawDebugPoint(World, Hit.ImpactPoint, 6.0f, FColor::Yellow, false, 1.0f);
#endif
    }
    else
    {
        // 没打到地面，保持原高度
#if WITH_EDITOR
        DrawDebugLine(World, Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
#endif
    }
}
