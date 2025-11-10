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


    // ========== 网络回调（由 NetSubsystem 唯一派发） ==========
    // 协议号你自己的定义：示例用 8000/8300/8400
    void OnMonsterData(uint32 Proto, FSimpleChannel* Channel);
    void OnMonsterState(uint32 Proto, FSimpleChannel* Channel);
    void OnMonsterMove(uint32 Proto, FSimpleChannel* Channel);

    // 对外查询
    AMMOARPGMonster*            FindMonsterById(int32 MonsterId) const;
    AMMOARPGNetEnemyController* FindMonsterCtlr (int32 MonsterId) const;

private:
    void BindNet();
    void UnbindNet();

    // 生成与回放
    void EnqueuePending(int32 MonsterId, const FQueuedMonsterMsg& Msg);
    void FlushPendingTo(AMMOARPGMonster* M, int32 MonsterId);
    void ApplyQueued(AMMOARPGMonster* M, const FQueuedMonsterMsg& Msg, bool bAuthoritative);
    void OnAuthoritativeTransform(int32 MonsterId, const FTransform& T, double ServerTimes);

    // 生成（同步版，编辑器/小资源可用）
    AMMOARPGMonster* SpawnMonsterByIdSync(int32 MonsterId, const FVector& Pos, const FRotator& Rot);

    // 定时清理排队
    void CleanupPending(float MaxHoldSec = 10.f);

private:
    // 运行时表
    TMap<int32, TWeakObjectPtr<AMMOARPGMonster>>            IdToMonster;
    TMap<int32, TWeakObjectPtr<AMMOARPGNetEnemyController>> IdToCtrl;

    // 未落地排队
    TMap<int32, TArray<FQueuedMonsterMsg>> PendingMsgs;
    TMap<int32, double>                    PendingFirstSeenSec;

    FTimerHandle PendingCleanupHandle;

    // 协议绑定（唯一消费者）：用你自己的 ID
    uint32 Proto_MonsterData  = 8000;
    uint32 Proto_MonsterState = 8300;
    uint32 Proto_MonsterMove  = 8400;

    // 标记是否已绑定（避免重复）
    bool bNetBound = false;
};
