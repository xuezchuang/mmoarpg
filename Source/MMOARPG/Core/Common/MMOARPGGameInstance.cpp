// Fill out your copyright notice in the Description page of Project Settings.
#include "MMOARPGGameInstance.h"
#include "Global/SimpleNetGlobalInfo.h"
#include "ThreadManage.h"
#include "DataTable/MonsterAnimTable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "MMOARPGMonster.h"
#include "MMOARPGNetEnemyController.h"
#include "MMOARPTool.h"

//#include "SimpleAdvancedAnimationBPLibrary.h"

int UMMOARPGGameInstance::nIndex = 0;

void UMMOARPGGameInstance::Init()
{	
	Super::Init();
	nIndex++;

	// 每 5 秒清一次排队
	GetWorld()->GetTimerManager().SetTimer(PendingCleanupHandle, [this]()
		{
			CleanupPending(10.f);
		}, 5.0f, true);

}

void UMMOARPGGameInstance::Tick(float DeltaTime)
{
	if(Client)
	{
		Client->Tick(DeltaTime);
	}

	GThread::Get()->Tick(DeltaTime);
}

TStatId UMMOARPGGameInstance::GetStatId() const
{
	return TStatId();
}

void UMMOARPGGameInstance::Shutdown()
{
	Super::Shutdown();
	nIndex--;
	if(Client)
	{
		FSimpleNetManage::Destroy(Client);
	}

	GThread::Destroy();
	//USimpleAdvancedAnimationBPLibrary::Destroy();
}

void UMMOARPGGameInstance::CreateClient()
{
	if (!Client)
	{
		FSimpleNetGlobalInfo::Get()->Init();

		Client = FSimpleNetManage::CreateManage(ESimpleNetLinkState::LINKSTATE_CONNET, ESimpleSocketType::SIMPLESOCKETTYPE_TCP);
	}
}

void UMMOARPGGameInstance::LinkServer()
{
	if(Client)
	{
		if(!Client->Init())
		{
			delete Client;
			Client = NULL;
		}
	}
}

void UMMOARPGGameInstance::LinkServer(const FSimpleAddr& InAddr)
{
	if (Client)
	{
		if (!Client->Init(InAddr))
		{
			delete Client;
			Client = NULL;
		}
	}
}

FSimpleNetManage* UMMOARPGGameInstance::GetClient()
{
	return Client;
}

FMMOARPGUserData& UMMOARPGGameInstance::GetUserData()
{
	return UserData;
}

FMMOARPGGateStatus& UMMOARPGGameInstance::GetGateStatus()
{
	return GateStatus;
}

void UMMOARPGGameInstance::LinkServer(const TCHAR* InIP, uint32 InPort)
{
	if(Client)
	{
		if(!Client->Init(InIP, InPort))
		{
			delete Client;
			Client = NULL;
		}
	}
}
UDataTable* UMMOARPGGameInstance::EnsureMonsterTableSync()
{
    if (DT_Monster_Loaded) return DT_Monster_Loaded;
    if (DT_Monster.IsNull()) return nullptr;

    DT_Monster_Loaded = DT_Monster.LoadSynchronous(); // 同步加载
    return DT_Monster_Loaded;
}

const FMonsterAnimRow* UMMOARPGGameInstance::GetMonsterRowSync(int32 MonsterId)
{
    UDataTable* Table = EnsureMonsterTableSync();
    if (!Table) return nullptr;

    static const FString Context(TEXT("MonsterRowLookup"));
    const FName RowName(*FString::FromInt(MonsterId));
    return Table->FindRow<FMonsterAnimRow>(RowName, Context);
}

static UClass* ResolveAnimBPClassSync(const TSoftObjectPtr<UAnimBlueprint>& SoftBP)
{
    if (SoftBP.IsNull()) return nullptr;
    if (SoftBP.IsValid())
    {
        return SoftBP.Get()->GeneratedClass;
    }
    UAnimBlueprint* BP = SoftBP.LoadSynchronous();        // 同步加载 AnimBP 资产
    return BP ? BP->GeneratedClass : nullptr;
}

bool UMMOARPGGameInstance::GetMonsterVisualSync(int32 MonsterId, FMonsterVisualResolved& Out)
{
    const auto* Row = GetMonsterRowSync(MonsterId);
    if (!Row) return false;

    // Mesh
    if (Row->Mesh.IsNull()) return false;
    USkeletalMesh* Mesh = Row->Mesh.IsValid() ? Row->Mesh.Get()
                                              : Row->Mesh.LoadSynchronous();
    if (!Mesh) return false;
    Out.SkeletalMesh = Mesh;

    // AnimBP → Class
    Out.AnimClass = ResolveAnimBPClassSync(Row->AnimBlueprint);

    // Attack montages
    Out.AttackMontages.Reset();
    Out.AttackMontages.Reserve(Row->AttackMontages.Num());
    for (const auto& SoftM : Row->AttackMontages)
    {
        UAnimMontage* M = SoftM.IsValid() ? SoftM.Get() : SoftM.LoadSynchronous();
        if (M) Out.AttackMontages.Add(M);
    }

    // Optional single montages
    Out.Idle  = Row->Idle.IsNull()  ? nullptr : (Row->Idle.IsValid()  ? Row->Idle.Get()  : Row->Idle.LoadSynchronous());
    Out.Hit   = Row->Hit.IsNull()   ? nullptr : (Row->Hit.IsValid()   ? Row->Hit.Get()   : Row->Hit.LoadSynchronous());
    Out.Death = Row->Death.IsNull() ? nullptr : (Row->Death.IsValid() ? Row->Death.Get() : Row->Death.LoadSynchronous());

    return true;
}

void UMMOARPGGameInstance::GetMonsterVisualAsync(
    int32 MonsterId,
    TFunction<void(bool bOk, const FMonsterAnimRow* Row, const FMonsterVisualResolved& Visual)> OnReady)
{
    FMonsterVisualResolved Result;

    UDataTable* Table = EnsureMonsterTableSync();
    if (!Table)
    {
        if (OnReady) OnReady(false, nullptr, Result);
        return;
    }

    const FMonsterAnimRow* Row = GetMonsterRowSync(MonsterId);
    if (!Row)
    {
        if (OnReady) OnReady(false, nullptr, Result);
        return;
    }

    // 收集需要异步加载的路径
	TArray<FSoftObjectPath> ToLoad;
	if (!Row->MonsterBlueprint.IsValid())
		ToLoad.Add(Row->MonsterBlueprint.ToSoftObjectPath());
	if (!Row->Mesh.IsValid())            
		ToLoad.Add(Row->Mesh.ToSoftObjectPath());
	if (!Row->AnimBlueprint.IsValid())  
		ToLoad.Add(Row->AnimBlueprint.ToSoftObjectPath());

    for (const auto& SoftM : Row->AttackMontages)
        if (!SoftM.IsValid()) ToLoad.Add(SoftM.ToSoftObjectPath());

    if (!Row->Idle.IsNull()  && !Row->Idle.IsValid())   ToLoad.Add(Row->Idle.ToSoftObjectPath());
    if (!Row->Hit.IsNull()   && !Row->Hit.IsValid())    ToLoad.Add(Row->Hit.ToSoftObjectPath());
    if (!Row->Death.IsNull() && !Row->Death.IsValid())  ToLoad.Add(Row->Death.ToSoftObjectPath());

    auto FinishResolve = [this, Row, OnReady](FMonsterVisualResolved ResultResolved)
    {
        if (OnReady) OnReady(true, Row, ResultResolved);
    };

    if (ToLoad.Num() == 0)
    {
        // 都已在内存，直接组装
        GetMonsterVisualSync(MonsterId, Result);
        FinishResolve(Result);
        return;
    }

    // 异步加载
    FStreamableManager& SM = UAssetManager::GetStreamableManager();
    SM.RequestAsyncLoad(ToLoad, [this, MonsterId, Row, FinishResolve]()
    {
        FMonsterVisualResolved R;
        const bool bOk = GetMonsterVisualSync(MonsterId, R);
        if (!bOk)
        {
            FMonsterVisualResolved Empty;
            FinishResolve(Empty);
            return;
        }
        FinishResolve(R);
    });
}

void UMMOARPGGameInstance::SpawnMonsterByIdAsync(int32 MonsterId, const FVector& Pos, const FRotator& Rot)
{
    // 先把蓝图类也加入到 GetMonsterVisualAsync 的加载队列里（见下方改造）
    const FMonsterAnimRow* Row = GetMonsterRowSync(MonsterId);
    if (!Row)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnMonsterByIdAsync: Row not found id=%d"), MonsterId);
        return;
    }

    GetMonsterVisualAsync(MonsterId,
        [this, MonsterId, Pos, Rot](bool bOk, const FMonsterAnimRow* ResolvedRow, const FMonsterVisualResolved& Visual)
        {
            if (!bOk || !ResolvedRow)
            {
                UE_LOG(LogTemp, Warning, TEXT("SpawnMonsterByIdAsync: Visual resolve failed id=%d"), MonsterId);
                return;
            }

            UWorld* W = GetWorld();
            if (!W) return;

            // 蓝图类也应已加载（见下方修改）
            UClass* BPClass = ResolvedRow->MonsterBlueprint.Get();
            if (!BPClass)
            {
                UE_LOG(LogTemp, Warning, TEXT("SpawnMonsterByIdAsync: MonsterBlueprint not ready id=%d"), MonsterId);
                return;
            }

            AMMOARPGMonster* Monster = W->SpawnActor<AMMOARPGMonster>(BPClass, Pos, Rot);
            if (!Monster) return;

            Monster->MonsterID = MonsterId;

            if (USkeletalMeshComponent* Skel = Monster->GetMesh())
            {
                if (Visual.SkeletalMesh) Skel->SetSkeletalMesh(Visual.SkeletalMesh);
                if (Visual.AnimClass)    Skel->SetAnimInstanceClass(Visual.AnimClass);
            }

            // 这里可把 Visual.AttackMontages/Idle/Hit/Death 注册到 Monster 的动作系统里
            Monster->UpdateHealthBar();
        });
}

AMMOARPGNetEnemyController* UMMOARPGGameInstance::FindMonsterCtlr(int32 MonsterId) const
{
	if (const TWeakObjectPtr<AMMOARPGNetEnemyController>* P = IdToCtrl.Find(MonsterId))
		return P->Get();
	return nullptr;
}

AMMOARPGMonster* UMMOARPGGameInstance::FindMonsterById(int32 MonsterId) const
{
	if (const TWeakObjectPtr<AMMOARPGMonster>* P = IdToMonster.Find(MonsterId))
		return P->Get();
	return nullptr;
}

// ====== 核心：权威落地与排队 ======
void UMMOARPGGameInstance::EnqueuePending(int32 MonsterId, const FQueuedMonsterMsg& Msg)
{
    PendingMsgs.FindOrAdd(MonsterId).Add(Msg);
    PendingFirstSeenSec.FindOrAdd(MonsterId) = FPlatformTime::Seconds();
}

void UMMOARPGGameInstance::OnAuthoritativeTransform(int32 MonsterId, const FTransform& T, double ServerTimeMs)
{
    if (AMMOARPGMonster* M = FindMonsterById(MonsterId))
    {
        // 已存在，按权威位置重置（可改为插值起点）
        M->SetActorTransform(T, false, nullptr, ETeleportType::TeleportPhysics);
        // 把排队消息回放
        FlushPendingTo(M, MonsterId);
        // 再套用一个“权威”消息（可选）
        FQueuedMonsterMsg Cur;
        Cur.ServerTimeMs  = ServerTimeMs;
        Cur.bHasTransform = true;
        Cur.Transform     = T;
        ApplyQueued(M, Cur, /*bAuthoritative*/true);
        return;
    }

    // 不存在 → 现在落地 + 回放
    AMMOARPGMonster* NewM = SpawnMonsterByIdSync(MonsterId, T.GetLocation(), T.Rotator());
    if (!NewM) return;

    // 重要：Deferred 的一定要 FinishSpawning
    NewM->FinishSpawning(T);

    // 记录
    IdToMonster.FindOrAdd(MonsterId) = NewM;
    if (AMMOARPGNetEnemyController* Ctl = Cast<AMMOARPGNetEnemyController>(NewM->GetController()))
    {
        IdToCtrl.FindOrAdd(MonsterId) = Ctl;
    }

    // 先回放排队里的旧消息（按时间升序）
    FlushPendingTo(NewM, MonsterId);

    // 再套用当前权威
    FQueuedMonsterMsg Cur;
    Cur.ServerTimeMs  = ServerTimeMs;
    Cur.bHasTransform = true;
    Cur.Transform     = T;
    ApplyQueued(NewM, Cur, /*bAuthoritative*/true);
}

void UMMOARPGGameInstance::FlushPendingTo(AMMOARPGMonster* M, int32 MonsterId)
{
    if (!M) return;
    TArray<FQueuedMonsterMsg>* Arr = PendingMsgs.Find(MonsterId);
    if (!Arr || Arr->Num() == 0) return;

    Arr->StableSort([](const FQueuedMonsterMsg& A, const FQueuedMonsterMsg& B)
    {
        return A.ServerTimeMs < B.ServerTimeMs;
    });

    for (const auto& Msg : *Arr)
    {
        ApplyQueued(M, Msg, /*bAuthoritative*/false);
    }
    PendingMsgs.Remove(MonsterId);
    PendingFirstSeenSec.Remove(MonsterId);
}

void UMMOARPGGameInstance::ApplyQueued(AMMOARPGMonster* M, const FQueuedMonsterMsg& Msg, bool bAuthoritative)
{
    if (!M) return;

    // 位姿（权威）
    if (Msg.bHasTransform)
    {
        M->SetActorTransform(Msg.Transform, false, nullptr, ETeleportType::TeleportPhysics);
    }

    //// HP/MaxHP
    //if (Msg.HP.IsSet())
    //{
    //    // 你自己的接口：Net_HealthUpdate / SetHP 等
    //    if (AMMOARPGNetEnemyController* Ctl = Cast<AMMOARPGNetEnemyController>(M->GetController()))
    //    {
    //        const int32 MaxHp = Msg.MaxHP.IsSet() ? Msg.MaxHP.GetValue() : (int32)M->TotalHealth;
    //        Ctl->Net_HealthUpdate(Msg.HP.GetValue(), MaxHp);
    //    }
    //    else
    //    {
    //        M->SetHP((float)Msg.HP.GetValue());
    //    }
    //}

    //// 逻辑状态（动画/AI）
    //if (Msg.LogicState.IsSet())
    //{
    //    if (AMMOARPGNetEnemyController* Ctl = Cast<AMMOARPGNetEnemyController>(M->GetController()))
    //    {
    //        Ctl->Net_SetLogicState(Msg.LogicState.GetValue(), Msg.ServerTimeMs);
    //    }
    //    else
    //    {
    //        M->ApplyLogicState(Msg.LogicState.GetValue(), Msg.ServerTimeMs);
    //    }
    //}

    // 移动目标（8400）
    if (Msg.bHasMoveTarget)
    {
        if (AMMOARPGNetEnemyController* Ctl = Cast<AMMOARPGNetEnemyController>(M->GetController()))
        {
            //Ctl->Net_MoveTo_At(Msg.MoveTarget, Msg.ServerTimeMs * 0.001 /*若你是毫秒，注意转成秒*/, 200.f, false);
			Ctl->Net_MoveTo(Msg.MoveTarget, 200.f, false);
        }
        else
        {
            // 无控制器时直接位移（慎用）
            //M->SetActorLocation(Msg.MoveTarget.GetValue(), false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
}

void UMMOARPGGameInstance::CleanupPending(float MaxHoldSec)
{
    const double Now = FPlatformTime::Seconds();
    for (auto It = PendingFirstSeenSec.CreateIterator(); It; ++It)
    {
        if (Now - It.Value() > MaxHoldSec)
        {
            const int32 MonsterId = It.Key();
            PendingMsgs.Remove(MonsterId);
            It.RemoveCurrent();
            UE_LOG(LogTemp, Warning, TEXT("Dropped pending msgs for MonsterId=%d due to timeout."), MonsterId);
        }
    }
}

// ====== 你的 Spawn 修正：记得 FinishSpawning 在调用处做 ======
AMMOARPGMonster* UMMOARPGGameInstance::SpawnMonsterByIdSync(int32 MonsterId, const FVector& Pos, const FRotator& Rot)
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    const FMonsterAnimRow* Row = GetMonsterRowSync(MonsterId);
    if (!Row) return nullptr;

    if (!Row->MonsterBlueprint.IsValid()) Row->MonsterBlueprint.LoadSynchronous();
    if (!Row->Mesh.IsValid())             Row->Mesh.LoadSynchronous();
    if (!Row->AnimBlueprint.IsValid())    Row->AnimBlueprint.LoadSynchronous();
    // … 省略 Montage/Idle/Hit/Death 的同步加载（同你原代码）

    UClass* BPClass = Row->MonsterBlueprint.Get();
    if (!BPClass) return nullptr;

    FTransform Tf(Rot, Pos);
    AMMOARPGMonster* Monster =
        World->SpawnActorDeferred<AMMOARPGMonster>(
            BPClass, Tf, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    if (!Monster) return nullptr;

    Monster->AIControllerClass = AMMOARPGNetEnemyController::StaticClass();
    Monster->MonsterID = MonsterId;

    if (USkeletalMeshComponent* Skel = Monster->GetMesh())
    {
        if (USkeletalMesh* Mesh = Row->Mesh.Get()) Skel->SetSkeletalMesh(Mesh);
        if (UAnimBlueprint* AnimBP = Row->AnimBlueprint.Get())
            if (UClass* AnimClass = AnimBP->GeneratedClass)
                Skel->SetAnimInstanceClass(AnimClass);
    }
    // 注意：FinishSpawning 在 OnAuthoritativeTransform 里做
    return Monster;
}

void UMMOARPGGameInstance::GI_OnMonsterData(const FMonsterDataPacket& P, double ServerTimeMs)
{
    // 是否能解析出世界位姿？
    bool bHasTransform = false;
    FTransform T;

    // 你已有的网格→世界换算（若包里就有世界坐标/旋转，直接用）
    if (/* P 含 GridX/Y 且可用 */ P.GridX >= 0 && P.GridY >= 0)
    {
        FS_GRID_BASE Grid; Grid.row = P.GridX; Grid.col = P.GridY;
        const FVector WorldPos = UMMOARPTool::GridToPosSimple(Grid, FVector::ZeroVector, C_WORLDMAP_ONE_GRID, true);
        const FRotator Rot = FRotator::ZeroRotator; // 如果包里有朝向，用它
        T = FTransform(Rot, WorldPos);
        bHasTransform = true;
    }

    // 已经存在 → 直接应用（若带权威位姿则重设）
    if (AMMOARPGMonster* M = FindMonsterById(P.Id))
    {
        FQueuedMonsterMsg Msg;
        Msg.ServerTimeMs = ServerTimeMs;
        Msg.bHasTransform = bHasTransform;
        if (bHasTransform) Msg.Transform = T;
        //if (P.HP >= 0)     Msg.HP = P.HP;
        //if (P.MaxHP >= 0)  Msg.MaxHP = P.MaxHP;
        ApplyQueued(M, Msg, /*bAuthoritative*/bHasTransform);
        return;
    }

    // 不存在：
    if (bHasTransform)
    {
        OnAuthoritativeTransform(P.Id, T, ServerTimeMs); // 将触发 Spawn + 回放
    }
    else
    {
        // 还没有位置，先排队
        FQueuedMonsterMsg Msg;
        Msg.ServerTimeMs = ServerTimeMs;
        //if (P.HP >= 0)    Msg.HP = P.HP;
        //if (P.MaxHP >= 0) Msg.MaxHP = P.MaxHP;
        EnqueuePending(P.Id, Msg);
    }
}

void UMMOARPGGameInstance::GI_OnMonsterState(int32 MonsterId, uint8 NewState, double ServerTimeMs)
{
    if (AMMOARPGMonster* M = FindMonsterById(MonsterId))
    {
        FQueuedMonsterMsg Msg; Msg.ServerTimeMs = ServerTimeMs; 
		//Msg.LogicState = NewState;
        ApplyQueued(M, Msg, /*bAuthoritative*/false);
        return;
    }
    // 未落地 → 排队
    FQueuedMonsterMsg Msg; Msg.ServerTimeMs = ServerTimeMs; 
	//Msg.LogicState = NewState;
    EnqueuePending(MonsterId, Msg);
}

void UMMOARPGGameInstance::GI_OnMonsterMove(const S_MOVE_ROBOT& Move, double ServerTimeMs)
{
    const int32 MonsterId = Move.robotindex; // 你的协议字段名
    // 从网格换算位姿（如果 Move 自带世界 pos/rot 就直接用）
    FS_GRID_BASE Grid; Grid.row = Move.x; Grid.col = Move.y;
    const FVector WorldPos = UMMOARPTool::GridToPosSimple(Grid, FVector::ZeroVector, C_WORLDMAP_ONE_GRID, true);
    const FRotator Rot = FRotator::ZeroRotator; // 若包里有朝向，用它
    const FTransform T(Rot, WorldPos);

    // 8400 一般可视为“带位置”的权威更新
    OnAuthoritativeTransform(MonsterId, T, ServerTimeMs);

    // 如果还想把“移动目标”也当作排队消息记录（可选）：
    // FQueuedMonsterMsg Q; Q.ServerTimeMs = ServerTimeMs; Q.MoveTarget = WorldPos;
    // if (AMMOARPGMonster* M = FindMonsterById(MonsterId)) ApplyQueued(M, Q, false);
    // else EnqueuePending(MonsterId, Q);
}
