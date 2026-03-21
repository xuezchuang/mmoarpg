// MonsterWorldSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MMOARPGType.h"            // 你的包体/数据结构声明
#include "Channel/SimpleChannel.h"
#include "MonsterWorldSubsystem.generated.h"

class UMMOARPGNetSubsystem;
class UMMOARPGGameInstance;
class AMMOARPGMonster;
class AMMOARPGNetEnemyController;

USTRUCT()
struct FQueuedMonsterMsg
{
    GENERATED_BODY()
    UPROPERTY() double   ServerTimes = 0.0;

    UPROPERTY() bool     bHasTransform = false;
    UPROPERTY() FTransform Transform;

    UPROPERTY() bool     bHasMoveTarget = false;
    UPROPERTY() FVector  MoveTarget = FVector::ZeroVector;

    // ……需要再加 HP/State 等就在这扩展
};

/** 将怪物生成/排队、网络应用，放在跟随关卡的 WorldSubsystem */
UCLASS()
class MMOARPG_API UMonsterWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    // 生命周期
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

    void RecvProtocol(uint32 Proto, FSimpleChannel* Channel);

    // 对外查询
    AMMOARPGMonster*            FindMonsterById(uint32 robotindex) const;
    AMMOARPGNetEnemyController* FindMonsterCtlr (uint32 robotindex) const;

	 void GetAllAliveMonsters(TArray<AMMOARPGMonster*>& OutMonsters) const;
private:
    void BindNet();
    void UnbindNet();

    // 生成与回放
    void EnqueuePending(uint32 robotindex, const FQueuedMonsterMsg& Msg);
    void FlushPendingTo(AMMOARPGMonster* M, uint32 robotindex);
    void ApplyQueued(AMMOARPGMonster* M, const FQueuedMonsterMsg& Msg, bool bAuthoritative);
	AMMOARPGMonster* SpawnAndSyncMonster(uint32 robotindex, int32 MonsterId, const FTransform& T, double ServerTimes);

    // 生成（同步版，编辑器/小资源可用）
    AMMOARPGMonster* SpawnMonsterByIdSync(int32 MonsterId, const FVector& Pos, const FRotator& Rot);

    // 定时清理排队
    void CleanupPending(float MaxHoldSec = 10.f);

	template<class T>
	T* GetGameInstance()
	{
		return GetWorld() != nullptr ? GetWorld()->GetGameInstance<T>() : nullptr;
	}

private:
    // 运行时表
    TMap<uint32, TWeakObjectPtr<AMMOARPGMonster>>            IdToMonster;
    TMap<uint32, TWeakObjectPtr<AMMOARPGNetEnemyController>> IdToCtrl;

    // 未落地排队
    TMap<uint32, TArray<FQueuedMonsterMsg>> PendingMsgs;
    TMap<uint32, double>                    PendingFirstSeenSec;

    FTimerHandle PendingCleanupHandle;

	TArray<uint32> Protos;

    // 标记是否已绑定（避免重复）
    bool bNetBound = false;
};
