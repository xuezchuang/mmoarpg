#include "MMOARPGNetEnemyController.h"
#include "Character/MMOARPGMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


AMMOARPGNetEnemyController::AMMOARPGNetEnemyController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true; // ��Ҫ Tick ����ֵ
}


void AMMOARPGNetEnemyController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // ����治���ƶ���ͼ��ֻ���ݷ�����״̬���� Transform
    if (auto* TempEnemyPawn = Cast<AMMOARPGMonster>(InPawn))
    {
        if (auto* Move = TempEnemyPawn->GetCharacterMovement())
        {
            Move->DisableMovement();         // ���� CharacterMovement ����ͼ����
            Move->bOrientRotationToMovement = false;
        }
        PrevVisualPos = TempEnemyPawn->GetActorLocation();
        bHasPrevVisualPos = true;
    }

    // ����κε��� AI �õļ�ʱ���������󴥣�
    GetWorldTimerManager().ClearAllTimersForObject(this);
}

void AMMOARPGNetEnemyController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!EnemyPawn) return;

    if (StateBuffer.Num() == 0)
    {
        // û״̬�������ٶȻ���0�����⶯������
        NetSpeed = FMath::FInterpTo(NetSpeed, 0.f, DeltaSeconds, 5.f);
        return;
    }

    const double ClientRenderTime = ComputeClientRenderTime();

    FNetMonsterState A, B;
    if (!SampleStates(ClientRenderTime, A, B))
    {
        // ֻ�����һ֡���ã������֡������λ
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

    // �޼�����״̬��������� 0.5s��
    const double Cut = ClientRenderTime - 0.5;
    int32 FirstUseful = 0;
    while (FirstUseful < StateBuffer.Num() && StateBuffer[FirstUseful].ServerTime < Cut) ++FirstUseful;
    if (FirstUseful > 0) StateBuffer.RemoveAt(0, FirstUseful, false);
}

// ================== ����ӿ� ==================

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

// ================== �ڲ�����ֵʵ�� ==================

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

    // ͬʱ���ȥ�أ�������
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
    return false; // ����������֡
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

    // ---- ʱ�����ֵ���� ----
    const double Den  = FMath::Max(B.ServerTime - A.ServerTime, KINDA_SMALL_NUMBER);
    float Alpha = static_cast<float>((ClientRenderTime - A.ServerTime) / Den);
    Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

    // ---- Ŀ��״̬�����Բ�ֵ��----
    const FVector  TargetPos = FMath::Lerp(A.Pos,   B.Pos,   Alpha);
    const FRotator TargetRot = FMath::Lerp(A.Rot,   B.Rot,   Alpha);
    float          TargetSpd = FMath::Lerp(A.Speed, B.Speed, Alpha);
    const ENetMonsterAction TargetAct = (Alpha < 0.5f) ? A.Action : B.Action;

    // ---- ������ʱֱ�ӡ�˲�ơ����루��ֹ��Ƥ�----
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

        // ��¼���綯�����ٶ�
        NetAction = TargetAct;
        NetSpeed  = FMath::Max(0.f, TargetSpd);

        // �������λ�ã�����һ֡���ٶȻ��˹��ơ�ʹ�ã�
        PrevVisualPos     = SnapPos;
        bHasPrevVisualPos = true;
        return;
    }

    // ---- Сƫ���ƽ����ֵ ----
    FVector  AimPos = TargetPos;
    if (bAdjustZOnApply)
    {
        AdjustZToGround(EnemyPawn, AimPos);
    }

    // λ��/��תƽ������ֵ���ʿɰ��ָе�����
    constexpr float LocInterpSpeed = 14.f;
    constexpr float RotInterpSpeed = 14.f;

    const FVector  NewPos = FMath::VInterpTo(CurPos, AimPos, DeltaSeconds, LocInterpSpeed);
    const FRotator NewRot = FMath::RInterpTo(EnemyPawn->GetActorRotation(), TargetRot, DeltaSeconds, RotInterpSpeed);

    EnemyPawn->SetActorLocation(NewPos);
    EnemyPawn->SetActorRotation(NewRot);

    // ---- �ٶȻ��˹��ƣ�����������ٶȲ��ɿ�/Ϊ��ʱ������Ļλ�ƹ��� ----
    if ((TargetSpd <= 0.f || !FMath::IsFinite(TargetSpd)) && bHasPrevVisualPos)
    {
        const float Moved = (NewPos - PrevVisualPos).Size();
        // ��ֹ�쳣��DeltaSeconds>0 ���ڿ�ͷ�ж�
        TargetSpd = Moved / DeltaSeconds;  // cm/s
    }

    // �ٶ�/����������ǯ�ƣ��ɸ�����Ŀ��Ҫ�������ޣ�
    constexpr float MaxReasonableSpeed = 2000.f; // cm/s ����
    TargetSpd = FMath::Clamp(FMath::IsFinite(TargetSpd) ? TargetSpd : 0.f, 0.f, MaxReasonableSpeed);

    // ������綯�������ţ����������ٶ�������������ѡ��
    ENetMonsterAction FinalAct = TargetAct;
    //if (FinalAct == ENetMonsterAction::None)
    //{
    //    FinalAct = (TargetSpd > 5.f) ? ENetMonsterAction::Move : ENetMonsterAction::Idle;
    //}

    NetAction = FinalAct;
    NetSpeed  = TargetSpd;

    // ---- ���¿���λ�û��� ----
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

    // �����΢��һ�㣬����ӵ�����������
    FVector Start = InOutPos + FVector(0.f, 0.f, 200.f);
    FVector End   = InOutPos - FVector(0.f, 0.f, 1000.f);

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(AdjustZToGround), false, tEnemyPawn);

    // ʹ�ÿ����߱��棨Visibility ͨ����������
    if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        // ���ĵ�������
        InOutPos.Z = Hit.ImpactPoint.Z;

#if WITH_EDITOR
        // ���ӻ������ߣ�ֻ�ڱ༭���ɼ���
        DrawDebugLine(World, Start, Hit.ImpactPoint, FColor::Green, false, 1.0f, 0, 1.0f);
        DrawDebugPoint(World, Hit.ImpactPoint, 6.0f, FColor::Yellow, false, 1.0f);
#endif
    }
    else
    {
        // û�򵽵��棬����ԭ�߶�
#if WITH_EDITOR
        DrawDebugLine(World, Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
#endif
    }
}
