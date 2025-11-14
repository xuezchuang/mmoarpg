// PlayerWorldSubsystem.cpp
#include "PlayerWorldSubsystem.h"
#include "MMOARPGNetSubsystem.h"
#include "MMOARPGGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Protocol/GameProtocol.h"
#include "MMOARPTool.h"
#include "NetPlay/BladeIINetPlayer.h"      // 远端玩家Pawn
#include "MMOARPGMacroType.h"
#include "MMOARPG.h"
#include "DataTable/MonsterAnimTable.h"

static bool IsGameplayMap_Player(const UWorld& W)
{
    if (!(W.WorldType == EWorldType::Game || W.WorldType == EWorldType::PIE))
        return false;

    const FString Short = FPackageName::GetShortName(W.GetMapName());
    if (Short.StartsWith(TEXT("Login")) || Short.StartsWith(TEXT("Gate")))
        return false;

    return true;
}

void UPlayerWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UPlayerWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    BindNet();

    if (IsGameplayMap_Player(InWorld))
    {
        // 通知服务器：我进入了某地图，拉取当前地图的玩家快照
        SEND_DATA(SP_EnterWorld);
    }

    // 每 5 秒清一次排队
    InWorld.GetTimerManager().SetTimer(
        PendingCleanupHandle,
        [this]() { CleanupPending(10.f); },
        5.0f, true
    );
}

void UPlayerWorldSubsystem::Deinitialize()
{
    UnbindNet();
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().ClearTimer(PendingCleanupHandle);
    }
    PendingMsgs.Empty();
    PendingFirstSeenSec.Empty();
    IdToPlayer.Empty();
    IdToCtrl.Empty();

    Super::Deinitialize();
}

void UPlayerWorldSubsystem::BindNet()
{
    UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    if (!GI) return;

    if (auto* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
    {
        // 这里列出你“玩家相关”的协议号（示例名，按你的工程替换）
        Protos =
        {
            //SP_RoleBaseInfo,   // 玩家进入本地图
            //SP_PlayerLeave,   // 玩家离开本地图
            SP_RoleBaseInfo,    // 基础数据（含坐标/朝向/外观等）
			SP_SelfMove,
            SP_OtherMove,    // 移动/位置更新
            SP_RoleHP,        // HP
            SP_RoleMP,        // MP
            //SP_PlayerState    // 自定义的状态
        };
        Net->RegisterUniqueHandlers(Protos, FProtocolHandler::CreateUObject(this, &UPlayerWorldSubsystem::RecvProtocol));
    }
}

void UPlayerWorldSubsystem::UnbindNet()
{
    UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    if (!GI) return;

    if (auto* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
    {
        Net->UnRegisterUniqueHandlers(Protos);
    }
}

void UPlayerWorldSubsystem::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
    switch (ProtocolNumber)
    {
    case SP_RoleBaseInfo:
    {
		S_ROLE_O_BASE RoleBase;
		SIMPLE_PROTOCOLS_RECEIVE(SP_RoleBaseInfo, RoleBase);

		const uint32 PlayerId = RoleBase.index;
		const FVector WorldPos(RoleBase.pos.x, RoleBase.pos.y, RoleBase.pos.z);
		
		if (ABladeIINetPlayer* PActor = FindPlayerById(PlayerId))
		{
			UE_LOG(MMOARPG, Error, TEXT("recv SP_RoleBaseInfo"));
		}
		else
		{
			const double ServerTimes = FPlatformTime::Seconds();
			SpawnMonsterByJobIdSync(PlayerId, RoleBase.innate.job, FTransform(FRotator::ZeroRotator, WorldPos), ServerTimes);
		}
		break;
    }
	case SP_OtherMove:
	{
		S_MOVE_ROLE rMove;
		SIMPLE_PROTOCOLS_RECEIVE(SP_OtherMove, rMove);

		const uint32 PlayerId = rMove.userindex;
		const FVector WorldPos(rMove.targetpos.x, rMove.targetpos.y, rMove.targetpos.z);
		const double ServerTimes = FPlatformTime::Seconds();

		if (ABladeIINetPlayer* PActor = FindPlayerById(PlayerId))
		{
			// 这里如果有自定义 NetPlayerController，调用它的插值/跟随接口
			if (AController* Ctl = PActor->GetController())
			{
				// 假设你有 Ctl->Net_MoveTo(WorldPos, false);
				// 如果没有，就先直接SetActorLocation或做你已有的移动方案
				PActor->SetActorLocation(WorldPos, false, nullptr, ETeleportType::None);
			}
			else
			{
				FQueuedPlayerMsg Msg; Msg.ServerTimes = ServerTimes;
				Msg.bHasMoveTarget = true; Msg.MoveTarget = WorldPos;
				EnqueuePending(PlayerId, Msg);
			}
		}
		else
		{
			SIMPLE_PROTOCOLS_SEND(SP_RoleBaseInfo, rMove.userindex);
		}
		break;
	}
    case SP_RoleHP:
    {
        struct T_RoleHP { uint64 playerId; int32 value; } RoleHP{};
        SIMPLE_PROTOCOLS_RECEIVE(SP_RoleHP, RoleHP);

        const double ServerTimes = FPlatformTime::Seconds();

        if (ABladeIINetPlayer* PActor = FindPlayerById(RoleHP.playerId))
        {
            FQueuedPlayerMsg Msg; Msg.ServerTimes = ServerTimes;
            Msg.bHasHP = true; Msg.HP = RoleHP.value;
            ApplyQueued(PActor, Msg, /*bAuthoritative*/ false);
        }
        else
        {
            //FQueuedPlayerMsg Msg; Msg.ServerTimes = ServerTimes;
            //Msg.bHasHP = true; Msg.HP = RoleHP.value;
            //EnqueuePending(RoleHP.playerId, Msg);
        }
        break;
    }
    default:
        break;
    }
}

ABladeIINetPlayer* UPlayerWorldSubsystem::FindPlayerById(uint32 PlayerId) const
{
    if (const TWeakObjectPtr<ABladeIINetPlayer>* P = IdToPlayer.Find(PlayerId)) return P->Get();
    return nullptr;
}

AController* UPlayerWorldSubsystem::FindPlayerCtlr(uint32 PlayerId) const
{
    if (const TWeakObjectPtr<AController>* P = IdToCtrl.Find(PlayerId)) return P->Get();
    return nullptr;
}

// PlayerWorldSubsystem.cpp

void UPlayerWorldSubsystem::GetAllOtherPlayers(TArray<ABladeIINetPlayer*>& OutPlayers) const
{
    OutPlayers.Reset();

    for (const auto& Pair : IdToPlayer)
    {
        if (ABladeIINetPlayer* P = Pair.Value.Get())
        {
            if (!P->IsPendingKill() && P->IsActorInitialized() && P->IsActorTickEnabled())
            {
                // 同样可以加 IsDead / IsOnline 等逻辑
                OutPlayers.Add(P);
            }
        }
    }
}


void UPlayerWorldSubsystem::EnqueuePending(uint32 PlayerId, const FQueuedPlayerMsg& Msg)
{
    PendingMsgs.FindOrAdd(PlayerId).Add(Msg);
    PendingFirstSeenSec.FindOrAdd(PlayerId) = FPlatformTime::Seconds();
}

void UPlayerWorldSubsystem::FlushPendingTo(ABladeIINetPlayer* P, uint32 PlayerId)
{
    if (!P) return;
    TArray<FQueuedPlayerMsg>* Arr = PendingMsgs.Find(PlayerId);
    if (!Arr || Arr->Num() == 0) return;

    Arr->StableSort([](const FQueuedPlayerMsg& A, const FQueuedPlayerMsg& B)
    {
        return A.ServerTimes < B.ServerTimes;
    });

    for (const auto& Msg : *Arr)
    {
        ApplyQueued(P, Msg, /*bAuthoritative*/false);
    }
    PendingMsgs.Remove(PlayerId);
    PendingFirstSeenSec.Remove(PlayerId);
}

void UPlayerWorldSubsystem::ApplyQueued(ABladeIINetPlayer* P, const FQueuedPlayerMsg& Msg, bool bAuthoritative)
{
    if (!P) return;

    if (Msg.bHasTransform)
    {
        P->SetActorTransform(Msg.Transform, false, nullptr, ETeleportType::TeleportPhysics);
    }

    if (Msg.bHasMoveTarget)
    {
        // 如果你有专门的控制器/插值系统，这里调用它；否则直接位置/插值到目标
        P->SetActorLocation(Msg.MoveTarget, false, nullptr, ETeleportType::None);
    }

    if (Msg.bHasHP)
    {
        // 写入你的属性系统/组件，然后发UI事件
        // P->SetHP(Msg.HP);
    }

    if (Msg.bHasMP)
    {
        // P->SetMP(Msg.MP);
    }

    if (Msg.bHasState)
    {
        // P->ApplyState(Msg.State);
    }
}

void UPlayerWorldSubsystem::SpawnMonsterByJobIdSync(uint32 PlayerId, uint32 jobId, const FTransform& T, double ServerTimes)
{
    ABladeIINetPlayer* NewP = SpawnPlayerProxySync(jobId, T.GetLocation(), T.Rotator());
    if (!NewP) return;

    IdToPlayer.FindOrAdd(PlayerId) = NewP;
    if (AController* Ctl = NewP->GetController())
    {
        IdToCtrl.FindOrAdd(PlayerId) = Ctl;
    }

    FlushPendingTo(NewP, PlayerId);

    FQueuedPlayerMsg Cur; Cur.ServerTimes = ServerTimes; Cur.bHasTransform = true; Cur.Transform = T;
    ApplyQueued(NewP, Cur, /*bAuthoritative*/true);
}

void UPlayerWorldSubsystem::OnPlayerEnterMap(uint32 PlayerId, const FVector& SpawnPos, const FRotator& SpawnRot)
{
    const FTransform T(SpawnRot, SpawnPos);
    //OnAuthoritativeTransform(PlayerId, T, FPlatformTime::Seconds());
}

void UPlayerWorldSubsystem::OnPlayerLeaveMap(uint32 PlayerId)
{
    if (ABladeIINetPlayer* P = FindPlayerById(PlayerId))
    {
        // 这里选择隐藏或销毁，按你项目需求
        P->Destroy();
    }
    IdToPlayer.Remove(PlayerId);
    IdToCtrl.Remove(PlayerId);
    PendingMsgs.Remove(PlayerId);
    PendingFirstSeenSec.Remove(PlayerId);
}

ABladeIINetPlayer* UPlayerWorldSubsystem::SpawnPlayerProxySync(uint32 jobId, const FVector& Pos, const FRotator& Rot)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	if (auto* GI = World->GetGameInstance<UMMOARPGGameInstance>())
	{
		const FCharacterAnimRow* Row = GI->GetPlayerRowSync(jobId);
		if (!Row) return nullptr;

		if (!Row->CharacterBlueprint.IsValid()) Row->CharacterBlueprint.LoadSynchronous();
		if (!Row->Mesh.IsValid())             Row->Mesh.LoadSynchronous();
		if (!Row->AnimBlueprint.IsValid())    Row->AnimBlueprint.LoadSynchronous();

		UClass* BPClass = Row->CharacterBlueprint.Get();
		if (!BPClass) return nullptr;

		FTransform Tf(Rot, Pos);
		ABladeIINetPlayer* Player =
			World->SpawnActorDeferred<ABladeIINetPlayer>(BPClass, Tf, nullptr, nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Player) return nullptr;

		// 初始化一些标识
		// Player->RemoteId = PlayerId; // 如果你有这个字段

		Player->FinishSpawning(Tf);

		return Player;
	}
	return NULL;
}

void UPlayerWorldSubsystem::CleanupPending(float MaxHoldSec)
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
