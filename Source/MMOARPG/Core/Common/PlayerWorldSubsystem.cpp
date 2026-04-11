// PlayerWorldSubsystem.cpp
#include "PlayerWorldSubsystem.h"
#include "MMOARPGNetSubsystem.h"
#include "MMOARPGGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Protocol/GameProtocol.h"
#include "MMOARPTool.h"
#include "NetPlay/BladeIINetPlayer.h"      // 杩滅鐜╁Pawn
#include "NetPlay/B2NetGameMode.h"
#include "MMOARPGMacroType.h"
#include "MMOARPG.h"
#include "DataTable/MonsterAnimTable.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimBlueprint.h"
#include "Stream/SimpleIOStream.h"
#include "EngineUtils.h"

static bool IsGameplayMap_Player(const UWorld& W)
{
    if (!(W.WorldType == EWorldType::Game || W.WorldType == EWorldType::PIE))
        return false;

    const FString Short = FPackageName::GetShortName(W.GetMapName());
    if (Short.StartsWith(TEXT("Login")) || Short.StartsWith(TEXT("Gate")))
        return false;

    return true;
}

namespace
{
	UClass* ResolveRemotePlayerClass(UWorld* World, const FCharacterAnimRow* Row)
	{
		if (Row)
		{
			if (!Row->CharacterBlueprint.IsValid())
			{
				Row->CharacterBlueprint.LoadSynchronous();
			}

			if (UClass* RowClass = Row->CharacterBlueprint.Get())
			{
				return RowClass;
			}
		}

		if (World)
		{
			if (ABladeIINetGameMode* GameMode = World->GetAuthGameMode<ABladeIINetGameMode>())
			{
				if (UClass* GameModeClass = GameMode->OtherCharacterClass.Get())
				{
					return GameModeClass;
				}
			}
		}

		static UClass* CachedFallbackClass = nullptr;
		if (!CachedFallbackClass)
		{
			CachedFallbackClass = LoadClass<ABladeIINetPlayer>(
				nullptr,
				TEXT("/Game/MetanoiaCombat/ThirdPerson/Blueprints/BP_NetPlayer.BP_NetPlayer_C"));
		}

		return CachedFallbackClass ? CachedFallbackClass : ABladeIINetPlayer::StaticClass();
	}

	void ApplyRemotePlayerVisuals(ABladeIINetPlayer* Player, const FCharacterAnimRow* Row)
	{
		if (!Player || !Row)
		{
			return;
		}

		if (!Row->Mesh.IsValid())
		{
			Row->Mesh.LoadSynchronous();
		}

		if (!Row->AnimBlueprint.IsValid())
		{
			Row->AnimBlueprint.LoadSynchronous();
		}

		if (USkeletalMeshComponent* Skel = Player->GetMesh())
		{
			if (USkeletalMesh* Mesh = Row->Mesh.Get())
			{
				Skel->SetSkeletalMesh(Mesh);
			}

			if (UAnimBlueprint* AnimBP = Row->AnimBlueprint.Get())
			{
				if (UClass* AnimClass = AnimBP->GeneratedClass)
				{
					Skel->SetAnimInstanceClass(AnimClass);
				}
			}
		}
	}
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
        // 閫氱煡鏈嶅姟鍣細鎴戣繘鍏ヤ簡鏌愬湴鍥撅紝鎷夊彇褰撳墠鍦板浘鐨勭帺瀹跺揩鐓?
        UE_LOG(MMOARPG, Display, TEXT("[GameplayNet] PlayerWorldSubsystem no longer auto-sends SP_EnterWorld"));
    }

    // 姣?5 绉掓竻涓€娆℃帓闃?
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
        // 杩欓噷鍒楀嚭浣犫€滅帺瀹剁浉鍏斥€濈殑鍗忚鍙凤紙绀轰緥鍚嶏紝鎸変綘鐨勫伐绋嬫浛鎹級
        Protos =
        {
            //SP_RoleBaseInfo,   // 鐜╁杩涘叆鏈湴鍥?
            //SP_PlayerLeave,   // 鐜╁绂诲紑鏈湴鍥?
            SP_RoleBaseInfo,    // 鍩虹鏁版嵁锛堝惈鍧愭爣/鏈濆悜/澶栬绛夛級
			SP_SelfMove,
            SP_OtherMove,    // 绉诲姩/浣嶇疆鏇存柊
            SP_RoleHP,        // HP
            SP_RoleMP,        // MP
            SP_RoleCurrency,  // Gold / diamonds
            //SP_PlayerState    // 鑷畾涔夌殑鐘舵€?
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
		UE_LOG(MMOARPG, Display, TEXT("[PlayerSync] Recv SP_RoleBaseInfo [uid:%u child:%u job:%u pos:(%d,%d,%d)]"),
			PlayerId, RoleBase.childcmd, RoleBase.innate.job, RoleBase.pos.x, RoleBase.pos.y, RoleBase.pos.z);

		if (RoleBase.childcmd != 0)
		{
			UE_LOG(MMOARPG, Warning, TEXT("[PlayerSync] Ignore SP_RoleBaseInfo with childcmd [%u] for uid [%u]"),
				RoleBase.childcmd, PlayerId);
			break;
		}

		if (IsLocalPlayerId(PlayerId))
		{
			UE_LOG(MMOARPG, Display, TEXT("[PlayerSync] Skip local player role base [uid:%u]"), PlayerId);
			break;
		}

		if (ABladeIINetPlayer* PActor = FindPlayerById(PlayerId))
		{
			UE_LOG(MMOARPG, Display, TEXT("[PlayerSync] Remote player already exists [uid:%u]"), PlayerId);
			PActor->UpdateBaseData(&RoleBase);
		}
		else
		{
			const double ServerTimes = FPlatformTime::Seconds();
			SpawnPlayerByJobIdSync(PlayerId, RoleBase.innate.job, FTransform(FRotator::ZeroRotator, WorldPos), ServerTimes);
		}
		break;
    }
	case SP_SelfMove:
	{
		TArray<uint8> Buffer;
		if (!Channel->Receive(Buffer))
		{
			break;
		}

		FSimpleIOStream Stream(Buffer);
		Stream.Seek(sizeof(FSimpleBunchHead));

		uint16 ChildCmd = 0;
		Stream >> ChildCmd;
		if (ChildCmd == 0)
		{
			break;
		}

		S_VECTOR3 ServerPos{};
		if (ChildCmd == 3000)
		{
			Stream >> ServerPos;
			UE_LOG(MMOARPG, Warning, TEXT("[PlayerSync] Recv SP_SelfMove correction [child:%u pos:(%d,%d,%d)]"),
				ChildCmd, ServerPos.x, ServerPos.y, ServerPos.z);

			if (APawn* LocalPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
			{
				LocalPawn->SetActorLocation(
					FVector(ServerPos.x, ServerPos.y, ServerPos.z),
					false,
					nullptr,
					ETeleportType::TeleportPhysics);
			}
		}
		else
		{
			UE_LOG(MMOARPG, Warning, TEXT("[PlayerSync] Recv SP_SelfMove error [child:%u]"), ChildCmd);
		}
		break;
	}
	case SP_OtherMove:
	{
		uint32 PlayerId = 0;
		int16 FaceRaw = 0;
		int32 Speed = 0;
		S_VECTOR3 TargetPos{};
		SIMPLE_PROTOCOLS_RECEIVE(SP_OtherMove, PlayerId, FaceRaw, Speed, TargetPos);

		const FVector WorldPos(TargetPos.x, TargetPos.y, TargetPos.z);
		const double ServerTimes = FPlatformTime::Seconds();
		const float Face = static_cast<float>(FaceRaw) / 100.f;

		if (IsLocalPlayerId(PlayerId))
		{
			break;
		}

		UE_LOG(MMOARPG, Display, TEXT("[PlayerSync] Recv SP_OtherMove [uid:%u target:(%d,%d,%d) speed:%d face:%d]"),
			PlayerId,
			TargetPos.x, TargetPos.y, TargetPos.z,
			Speed, FaceRaw);

		if (ABladeIINetPlayer* PActor = FindPlayerById(PlayerId))
		{
			S_MOVE_ROLE MoveData{};
			MoveData.userindex = PlayerId;
			MoveData.face = FaceRaw;
			MoveData.speed = Speed;
			MoveData.targetpos = TargetPos;
			PActor->UpdateMoveData(&MoveData);
			break;
		}
		else
		{
			FQueuedPlayerMsg Msg; Msg.ServerTimes = ServerTimes;
			Msg.bHasMoveTarget = true; Msg.MoveTarget = WorldPos;
			EnqueuePending(PlayerId, Msg);
			UE_LOG(MMOARPG, Display, TEXT("[PlayerSync] Missing remote player for move, request role base [uid:%u]"), PlayerId);
			SIMPLE_PROTOCOLS_SEND(SP_RoleBaseInfo, PlayerId);
		}
		break;
	}
	case SP_RoleCurrency:
	{
		uint32 Gold = 0;
		uint32 Diamonds = 0;
		SIMPLE_PROTOCOLS_RECEIVE(SP_RoleCurrency, Gold, Diamonds);

		if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
		{
			FMMOARPGUserData& UserData = GI->GetUserData();
			UserData.base.econ.gold = Gold;
			UserData.base.econ.diamonds = Diamonds;
		}

		UE_LOG(MMOARPG, Display, TEXT("[PlayerSync] Recv SP_RoleCurrency [gold:%u diamonds:%u]"),
			Gold, Diamonds);
		break;
	}
	case SP_RoleHP:
    {
        struct T_RoleHP { uint64 playerId; int32 value; } RoleHP{};
        SIMPLE_PROTOCOLS_RECEIVE(SP_RoleHP, RoleHP);

        const double ServerTimes = FPlatformTime::Seconds();

		if (IsLocalPlayerId(static_cast<uint32>(RoleHP.playerId)))
		{
			if (UWorld* World = GetWorld())
			{
				if (UMMOARPGGameInstance* GI = World->GetGameInstance<UMMOARPGGameInstance>())
				{
					GI->GetUserData().base.life.hp = RoleHP.value;
				}
			}
			break;
		}

        if (ABladeIINetPlayer* PActor = FindPlayerById(RoleHP.playerId))
        {
            FQueuedPlayerMsg Msg; Msg.ServerTimes = ServerTimes;
            Msg.bHasHP = true; Msg.HP = RoleHP.value;
            ApplyQueued(PActor, Msg, /*bAuthoritative*/ false);
        }
        else
        {
            FQueuedPlayerMsg Msg; Msg.ServerTimes = ServerTimes;
            Msg.bHasHP = true; Msg.HP = RoleHP.value;
            EnqueuePending(static_cast<uint32>(RoleHP.playerId), Msg);
        }
        break;
    }
	case SP_RoleMP:
	{
		struct T_RoleMP { uint64 playerId; int32 value; } RoleMP{};
		SIMPLE_PROTOCOLS_RECEIVE(SP_RoleMP, RoleMP);

		if (IsLocalPlayerId(static_cast<uint32>(RoleMP.playerId)))
		{
			if (UWorld* World = GetWorld())
			{
				if (UMMOARPGGameInstance* GI = World->GetGameInstance<UMMOARPGGameInstance>())
				{
					GI->GetUserData().base.life.mp = RoleMP.value;
				}
			}
		}
		break;
	}
    default:
        break;
    }
}

ABladeIINetPlayer* UPlayerWorldSubsystem::FindPlayerById(uint32 PlayerId)
{
	if (TObjectPtr<ABladeIINetPlayer>* Cached = IdToPlayer.Find(PlayerId))
	{
		if (ABladeIINetPlayer* Player = Cached->Get())
		{
			return Player;
		}

		IdToPlayer.Remove(PlayerId);
		IdToCtrl.Remove(PlayerId);
		UE_LOG(MMOARPG, Warning, TEXT("[PlayerSync] Cached remote player handle expired [uid:%u]"), PlayerId);
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	for (TActorIterator<ABladeIINetPlayer> It(World); It; ++It)
	{
		ABladeIINetPlayer* Candidate = *It;
		if (!IsValid(Candidate))
		{
			continue;
		}

		if (Candidate->GetRemotePlayerId() != PlayerId)
		{
			continue;
		}

		IdToPlayer.FindOrAdd(PlayerId) = Candidate;
		if (AController* Controller = Candidate->GetController())
		{
			IdToCtrl.FindOrAdd(PlayerId) = Controller;
		}

		UE_LOG(MMOARPG, Display, TEXT("[PlayerSync] Restored remote player cache from world [uid:%u actor:%s]"),
			PlayerId, *GetNameSafe(Candidate));
		return Candidate;
	}

	return nullptr;
}

AController* UPlayerWorldSubsystem::FindPlayerCtlr(uint32 PlayerId) const
{
    if (const TObjectPtr<AController>* P = IdToCtrl.Find(PlayerId)) return P->Get();
    return nullptr;
}

bool UPlayerWorldSubsystem::IsLocalPlayerId(uint32 PlayerId) const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UMMOARPGGameInstance* GI = World->GetGameInstance<UMMOARPGGameInstance>())
		{
			const int32 LocalUserIndex = GI->GetLocalUserIndex();
			return LocalUserIndex != INDEX_NONE && PlayerId == static_cast<uint32>(LocalUserIndex);
		}
	}

	return false;
}

// PlayerWorldSubsystem.cpp

void UPlayerWorldSubsystem::GetAllOtherPlayers(TArray<ABladeIINetPlayer*>& OutPlayers) const
{
    OutPlayers.Reset();

    for (const auto& Pair : IdToPlayer)
    {
        if (ABladeIINetPlayer* P = Pair.Value.Get())
        {
			if (IsValid(P) && P->IsActorInitialized() && P->IsActorTickEnabled())
            {
                // 鍚屾牱鍙互鍔?IsDead / IsOnline 绛夐€昏緫
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
        // 濡傛灉浣犳湁涓撻棬鐨勬帶鍒跺櫒/鎻掑€肩郴缁燂紝杩欓噷璋冪敤瀹冿紱鍚﹀垯鐩存帴浣嶇疆/鎻掑€煎埌鐩爣
        P->SetActorLocation(Msg.MoveTarget, false, nullptr, ETeleportType::None);
    }

    if (Msg.bHasHP)
    {
        // 鍐欏叆浣犵殑灞炴€х郴缁?缁勪欢锛岀劧鍚庡彂UI浜嬩欢
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

void UPlayerWorldSubsystem::SpawnPlayerByJobIdSync(uint32 PlayerId, uint32 jobId, const FTransform& T, double ServerTimes)
{
    ABladeIINetPlayer* NewP = SpawnPlayerProxySync(jobId, T.GetLocation(), T.Rotator());
    if (!NewP)
	{
		UE_LOG(MMOARPG, Error, TEXT("[PlayerSync] Failed to spawn remote player [uid:%u job:%u pos:(%.1f,%.1f,%.1f)]"),
			PlayerId, jobId, T.GetLocation().X, T.GetLocation().Y, T.GetLocation().Z);
		return;
	}

	NewP->SetRemotePlayerId(PlayerId);
    IdToPlayer.FindOrAdd(PlayerId) = NewP;
    if (AController* Ctl = NewP->GetController())
    {
        IdToCtrl.FindOrAdd(PlayerId) = Ctl;
    }

	UE_LOG(MMOARPG, Display, TEXT("[PlayerSync] Spawned remote player [uid:%u job:%u actor:%s]"),
		PlayerId, jobId, *GetNameSafe(NewP));

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
        // 杩欓噷閫夋嫨闅愯棌鎴栭攢姣侊紝鎸変綘椤圭洰闇€姹?
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
		if (!Row)
		{
			UE_LOG(MMOARPG, Warning, TEXT("[PlayerSync] Missing player row for job [%u], use fallback remote player class"), jobId);
		}

		UClass* BPClass = ResolveRemotePlayerClass(World, Row);
		if (!BPClass)
		{
			UE_LOG(MMOARPG, Error, TEXT("[PlayerSync] ResolveRemotePlayerClass failed for job [%u]"), jobId);
			return nullptr;
		}

		FTransform Tf(Rot, Pos);
		ABladeIINetPlayer* Player =
			World->SpawnActorDeferred<ABladeIINetPlayer>(BPClass, Tf, nullptr, nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Player)
		{
			UE_LOG(MMOARPG, Error, TEXT("[PlayerSync] SpawnActorDeferred failed for class [%s] job [%u]"),
				*GetNameSafe(BPClass), jobId);
			return nullptr;
		}

		// 鍒濆鍖栦竴浜涙爣璇?
		// Player->RemoteId = PlayerId; // 濡傛灉浣犳湁杩欎釜瀛楁

		ApplyRemotePlayerVisuals(Player, Row);

		Player->FinishSpawning(Tf);
		UE_LOG(MMOARPG, Display, TEXT("[PlayerSync] SpawnPlayerProxySync success [job:%u class:%s pos:(%.1f,%.1f,%.1f)]"),
			jobId, *GetNameSafe(BPClass), Pos.X, Pos.Y, Pos.Z);

		return Player;
	}
	UE_LOG(MMOARPG, Error, TEXT("[PlayerSync] SpawnPlayerProxySync failed because GameInstance is null [job:%u]"), jobId);
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


