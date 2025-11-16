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
#include "NetPlay/BladeIINetCharacter.h"
#include "MMOARPGMacroType.h"
#include "MMOARPG.h"

// 你项目里的通道类型
class FSimpleChannel;

static bool IsGameplayMap(const UWorld& W)
{
	// 1) 只认 Game/PIE 世界
	if (!(W.WorldType == EWorldType::Game || W.WorldType == EWorldType::PIE))
		return false;

	//// 2) 用 GameState / PlayerController 判别（客户端上没有 GameMode，用它不稳）
	//if (const AGameStateBase* GS = W.GetGameState())
	//{
	//	if (GS->IsA(ABladeIINetGameState::StaticClass()))
	//		return true;
	//}

	// 或：看本地玩家控制器类型（如果你登陆/门厅用不同的 PC）
	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(const_cast<UWorld*>(&W), 0))
	{
		if (PC->IsA(ABladeIINetCharacter::StaticClass()))
			return true;
	}

	// 3) 兜底：地图名约定（例如 Login_* / Gate_* 不是游戏地图）
	const FString Short = FPackageName::GetShortName(W.GetMapName());
	if (Short.StartsWith(TEXT("Login")) || Short.StartsWith(TEXT("Gate")))
		return false;

	return true; // 其他默认当作游戏地图

}
void UMonsterWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UMonsterWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    BindNet();

	if (IsGameplayMap(InWorld))
	{
		SEND_DATA(SP_EnterWorld);
	}

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
		Protos =
		{
			SP_RoleHP,
			SP_RoleMP,
			SP_RoleState,
			SP_MonsterData,
			SP_MonsterMove,
			SP_MonsterState
		};
		Net->RegisterUniqueHandlers(Protos, FProtocolHandler::CreateUObject(this, &UMonsterWorldSubsystem::RecvProtocol));
	}
}

void UMonsterWorldSubsystem::UnbindNet()
{
    UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    if (!GI) return;

	if (auto* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
	{
		Net->UnRegisterUniqueHandlers(Protos);
	}
}


void UMonsterWorldSubsystem::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
	switch (ProtocolNumber)
	{
	case SP_MonsterData:
	{
		FMonsterDataPacket P{}; // 你的结构
		SIMPLE_PROTOCOLS_RECEIVE(SP_MonsterData, P);

		const double ServerTimes = FPlatformTime::Seconds(); // 或包里带的 server time

		const FVector WorldPos(P.GridX, P.GridY, 0.0);
		FTransform T = FTransform(FRotator::ZeroRotator, WorldPos);
		
		if (AMMOARPGMonster* M = FindMonsterById(P.Id))
		{
			UE_LOG(MMOARPG, Error, TEXT("recv SP_MonsterData"));
		}
		else
		{
			AMMOARPGMonster* NewMonster = SpawnAndSyncMonster(P.Id, T, ServerTimes);
			if (NewMonster)
			{
				NewMonster->Info.CurrentHealth = P.Hp;
				NewMonster->Info.TotalHealth = P.TolHp;
				NewMonster->UpdateHealthBar();
			}
		}
		break;
	}
	case SP_MonsterState:
	{
		int32 MonsterId = 0; uint8 NewState = 0;
		SIMPLE_PROTOCOLS_RECEIVE(SP_MonsterState, MonsterId, NewState);

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
		break;
	}
	case SP_MonsterMove:
	{
		S_MOVE_ROBOT Move{}; // 你的移动结构
		SIMPLE_PROTOCOLS_RECEIVE(SP_MonsterMove, Move);

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
			SIMPLE_PROTOCOLS_SEND(SP_MonsterData, MonsterId);
			//SpawnAndSyncMonster(MonsterId, FTransform(FRotator::ZeroRotator, WorldPos), ServerTimes);
		}
		break;
	}
	case SP_MonsterAttack:
	{
		//uint32  index;
		//int32	value;
		//SIMPLE_PROTOCOLS_RECEIVE(SP_MonsterAttack, index, value);
		break;
	}
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

void UMonsterWorldSubsystem::GetAllAliveMonsters(TArray<AMMOARPGMonster*>& OutMonsters) const
{
    OutMonsters.Reset();

    for (const auto& Pair : IdToMonster)
    {
        if (AMMOARPGMonster* M = Pair.Value.Get())
        {
			if (IsValid(M) && M->IsActorInitialized() && M->IsActorTickEnabled())
			{
				OutMonsters.Add(M);
			}
        }
    }
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

AMMOARPGMonster* UMonsterWorldSubsystem::SpawnAndSyncMonster(int32 MonsterId, const FTransform& T, double ServerTimes)
{
    AMMOARPGMonster* NewM = SpawnMonsterByIdSync(MonsterId, T.GetLocation(), T.Rotator());
    if (!NewM) 
	{
		return NULL;
	}

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
	return NewM;
}

AMMOARPGMonster* UMonsterWorldSubsystem::SpawnMonsterByIdSync(int32 MonsterId, const FVector& Pos, const FRotator& Rot)
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

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


