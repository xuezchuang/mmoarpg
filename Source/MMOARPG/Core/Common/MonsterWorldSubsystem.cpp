// MonsterWorldSubsystem.cpp
#include "MonsterWorldSubsystem.h"
#include "MMOARPGNetSubsystem.h"
#include "MMOARPGGameInstance.h"
#include "MMOARPGMonster.h"
#include "MMOARPGNetEnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "Protocol/GameProtocol.h"
#include "MMOARPTool.h"
#include "../DataTable/MonsterAnimTable.h"

// 你项目里的通道类型
class FSimpleChannel;

void UMonsterWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UMonsterWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    BindNet();

    // 每 5 秒清一次排队
    InWorld.GetTimerManager().SetTimer(
        PendingCleanupHandle,
        [this]() { CleanupPending(10.f); },
        5.0f, true
    );
}

void UMonsterWorldSubsystem::Deinitialize()
{
    UnbindNet();
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().ClearTimer(PendingCleanupHandle);
    }
    PendingMsgs.Empty();
    PendingFirstSeenSec.Empty();
    IdToMonster.Empty();
    IdToCtrl.Empty();

	
    Super::Deinitialize();
}

void UMonsterWorldSubsystem::BindNet()
{
	UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	if (!GI) return;

	if (auto* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
	{
		// 按你的 NetSubsystem API 绑定（示例：唯一处理者/或多播）
		Net->RegisterUniqueHandler(SP_MonsterData,FProtocolHandler::CreateUObject(this, &UMonsterWorldSubsystem::OnMonsterData));

		Net->RegisterUniqueHandler(SP_MonsterMove,FProtocolHandler::CreateUObject(this, &UMonsterWorldSubsystem::OnMonsterMove));

	}
}

void UMonsterWorldSubsystem::UnbindNet()
{
    UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    if (!GI) return;

    if (auto* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
    {
        Net->UnRegisterUniqueHandler(SP_MonsterData);
        Net->UnRegisterUniqueHandler(SP_MonsterMove);
    }
}


// 8000：怪物数据（可能带初始位姿）
void UMonsterWorldSubsystem::OnMonsterData(uint32 /*Proto*/, FSimpleChannel* Channel)
{
    // 示例：照你现有读取方式
    TArray<uint8> Buf; Channel->Receive(Buf);
    FSimpleIOStream Ar(Buf); Ar.Seek(sizeof(FSimpleBunchHead));

    FMonsterDataPacket P{}; // 你的结构
    // 按你的协议把 P 读出来……
    // Ar >> P....

    const double ServerTimes = FPlatformTime::Seconds(); // 或包里带的 server time

    bool bHasTransform = false;
    FTransform T;

    // 示例：如果包里是 Grid，转世界坐标
    if (P.GridX >= 0 && P.GridY >= 0)
    {
        FS_GRID_BASE Grid; Grid.row = P.GridX; Grid.col = P.GridY;
        const FVector WorldPos = UMMOARPTool::GridToPosSimple(Grid, FVector::ZeroVector, C_WORLDMAP_ONE_GRID, true);
        T = FTransform(FRotator::ZeroRotator, WorldPos);
        bHasTransform = true;
    }

    if (AMMOARPGMonster* M = FindMonsterById(P.Id))
    {
        FQueuedMonsterMsg Msg;
        Msg.ServerTimes = ServerTimes;
        Msg.bHasTransform = bHasTransform;
        if (bHasTransform) Msg.Transform = T;
        ApplyQueued(M, Msg, /*bAuthoritative*/ bHasTransform);
        return;
    }

    if (bHasTransform)
    {
        OnAuthoritativeTransform(P.Id, T, ServerTimes);
    }
    else
    {
        FQueuedMonsterMsg Msg; Msg.ServerTimes = ServerTimes;
        EnqueuePending(P.Id, Msg);
    }
}

// 8300：状态
void UMonsterWorldSubsystem::OnMonsterState(uint32 /*Proto*/, FSimpleChannel* Channel)
{
    TArray<uint8> Buf; Channel->Receive(Buf);
    FSimpleIOStream Ar(Buf); Ar.Seek(sizeof(FSimpleBunchHead));

    int32 MonsterId = 0; uint8 NewState = 0;
    // Ar >> MonsterId >> NewState;

    const double ServerTimes = FPlatformTime::Seconds();

    if (AMMOARPGMonster* M = FindMonsterById(MonsterId))
    {
        FQueuedMonsterMsg Msg; Msg.ServerTimes = ServerTimes;
        // Msg.填状态
        ApplyQueued(M, Msg, /*bAuthoritative*/ false);
    }
    else
    {
        FQueuedMonsterMsg Msg; Msg.ServerTimes = ServerTimes;
        // Msg.填状态
        EnqueuePending(MonsterId, Msg);
    }
}

// 8400：移动
void UMonsterWorldSubsystem::OnMonsterMove(uint32 /*Proto*/, FSimpleChannel* Channel)
{
    TArray<uint8> Buf; Channel->Receive(Buf);
    FSimpleIOStream Ar(Buf); Ar.Seek(sizeof(FSimpleBunchHead));

    S_MOVE_ROBOT Move{}; // 你的移动结构
    // Ar >> Move ...

    const int32 MonsterId = Move.robotindex;
    const FVector WorldPos(Move.x, Move.y, Move.z);
    const double ServerTimes = FPlatformTime::Seconds();

    if (AMMOARPGMonster* M = FindMonsterById(MonsterId))
    {
        if (auto* Ctl = Cast<AMMOARPGNetEnemyController>(M->GetController()))
        {
            Ctl->Net_MoveTo(WorldPos, false);
        }
        else
        {
            FQueuedMonsterMsg Msg; Msg.ServerTimes = ServerTimes;
            Msg.bHasMoveTarget = true; Msg.MoveTarget = WorldPos;
            EnqueuePending(MonsterId, Msg);
        }
    }
    else
    {
        // 首次见到，用移动包位置当权威位姿生成
        OnAuthoritativeTransform(MonsterId, FTransform(FRotator::ZeroRotator, WorldPos), ServerTimes);
    }
}

AMMOARPGMonster* UMonsterWorldSubsystem::FindMonsterById(int32 MonsterId) const
{
    if (const TWeakObjectPtr<AMMOARPGMonster>* P = IdToMonster.Find(MonsterId)) return P->Get();
    return nullptr;
}

AMMOARPGNetEnemyController* UMonsterWorldSubsystem::FindMonsterCtlr(int32 MonsterId) const
{
    if (const TWeakObjectPtr<AMMOARPGNetEnemyController>* P = IdToCtrl.Find(MonsterId)) return P->Get();
    return nullptr;
}

void UMonsterWorldSubsystem::EnqueuePending(int32 MonsterId, const FQueuedMonsterMsg& Msg)
{
    PendingMsgs.FindOrAdd(MonsterId).Add(Msg);
    PendingFirstSeenSec.FindOrAdd(MonsterId) = FPlatformTime::Seconds();
}

void UMonsterWorldSubsystem::FlushPendingTo(AMMOARPGMonster* M, int32 MonsterId)
{
    if (!M) return;
    TArray<FQueuedMonsterMsg>* Arr = PendingMsgs.Find(MonsterId);
    if (!Arr || Arr->Num() == 0) return;

    Arr->StableSort([](const FQueuedMonsterMsg& A, const FQueuedMonsterMsg& B)
    {
        return A.ServerTimes < B.ServerTimes;
    });

    for (const auto& Msg : *Arr)
    {
        ApplyQueued(M, Msg, /*bAuthoritative*/false);
    }
    PendingMsgs.Remove(MonsterId);
    PendingFirstSeenSec.Remove(MonsterId);
}

void UMonsterWorldSubsystem::ApplyQueued(AMMOARPGMonster* M, const FQueuedMonsterMsg& Msg, bool bAuthoritative)
{
    if (!M) return;

    if (Msg.bHasTransform)
    {
        M->SetActorTransform(Msg.Transform, false, nullptr, ETeleportType::TeleportPhysics);
    }

    if (Msg.bHasMoveTarget)
    {
        if (auto* Ctl = Cast<AMMOARPGNetEnemyController>(M->GetController()))
        {
            Ctl->Net_MoveTo(Msg.MoveTarget, false);
        }
    }
}

void UMonsterWorldSubsystem::OnAuthoritativeTransform(int32 MonsterId, const FTransform& T, double ServerTimes)
{
    if (AMMOARPGMonster* Exist = FindMonsterById(MonsterId))
    {
        Exist->SetActorTransform(T, false, nullptr, ETeleportType::TeleportPhysics);
        FlushPendingTo(Exist, MonsterId);
        return;
    }

    AMMOARPGMonster* NewM = SpawnMonsterByIdSync(MonsterId, T.GetLocation(), T.Rotator());
    if (!NewM) return;

    // 记录
    IdToMonster.FindOrAdd(MonsterId) = NewM;
    if (AMMOARPGNetEnemyController* Ctl = Cast<AMMOARPGNetEnemyController>(NewM->GetController()))
    {
        IdToCtrl.FindOrAdd(MonsterId) = Ctl;
    }

    // 先回放排队
    FlushPendingTo(NewM, MonsterId);

    // 再应用权威
    FQueuedMonsterMsg Cur; Cur.ServerTimes = ServerTimes; Cur.bHasTransform = true; Cur.Transform = T;
    ApplyQueued(NewM, Cur, /*bAuthoritative*/true);
}

AMMOARPGMonster* UMonsterWorldSubsystem::SpawnMonsterByIdSync(int32 MonsterId, const FVector& Pos, const FRotator& Rot)
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    // 继续复用 GI 的“解析 DataTable + 资源加载”逻辑，避免重复代码
    if (auto* GI = World->GetGameInstance<UMMOARPGGameInstance>())
    {
        const FMonsterAnimRow* Row = GI->GetMonsterRowSync(MonsterId);
        if (!Row) return nullptr;

        if (!Row->MonsterBlueprint.IsValid()) Row->MonsterBlueprint.LoadSynchronous();
        if (!Row->Mesh.IsValid())             Row->Mesh.LoadSynchronous();
        if (!Row->AnimBlueprint.IsValid())    Row->AnimBlueprint.LoadSynchronous();

        UClass* BPClass = Row->MonsterBlueprint.Get();
        if (!BPClass) return nullptr;

        FTransform Tf(Rot, Pos);
        AMMOARPGMonster* Monster =
            World->SpawnActorDeferred<AMMOARPGMonster>(BPClass, Tf, nullptr, nullptr,
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
        if (!Monster) return nullptr;

        Monster->AutoPossessAI   = EAutoPossessAI::PlacedInWorldOrSpawned;
        Monster->AIControllerClass = AMMOARPGNetEnemyController::StaticClass();
        Monster->MonsterID = MonsterId;

        if (USkeletalMeshComponent* Skel = Monster->GetMesh())
        {
            if (USkeletalMesh* Mesh = Row->Mesh.Get()) Skel->SetSkeletalMesh(Mesh);
            if (UAnimBlueprint* AnimBP = Row->AnimBlueprint.Get())
                if (UClass* AnimClass = AnimBP->GeneratedClass)
                    Skel->SetAnimInstanceClass(AnimClass);
        }

        Monster->FinishSpawning(Tf);
        return Monster;
    }
    return nullptr;
}

void UMonsterWorldSubsystem::CleanupPending(float MaxHoldSec)
{
    const double Now = FPlatformTime::Seconds();
    for (auto It = PendingFirstSeenSec.CreateIterator(); It; ++It)
    {
        if (Now - It.Value() > MaxHoldSec)
        {
            PendingMsgs.Remove(It.Key());
            It.RemoveCurrent();
        }
    }
}
