// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SimpleNetManage.h"
#include "MMOARPGType.h"
#include "Engine/DataTable.h"
#include "MMOARPGGameInstance.generated.h"

struct FMonsterAnimRow;
class AMMOARPGMonster;
class AMMOARPGNetEnemyController;

USTRUCT()
struct FMonsterVisualResolved
{
    GENERATED_BODY()

    USkeletalMesh* SkeletalMesh = nullptr;
    UClass* AnimClass = nullptr;
    TArray<UAnimMontage*> AttackMontages;
    UAnimMontage* Idle  = nullptr;
    UAnimMontage* Hit   = nullptr;
    UAnimMontage* Death = nullptr;
};

USTRUCT()
struct FQueuedMonsterMsg
{
    GENERATED_BODY()

    // 用于排序回放
    UPROPERTY() double ServerTimeMs = 0.0;

    // 是否带有权威位姿（拿到后将触发真正 Spawn 或权威重设）
    UPROPERTY() bool bHasTransform = false;
    UPROPERTY() FTransform Transform;

    UPROPERTY() bool  bHasHP     = false;
    UPROPERTY() int32 HP         = 0;

    UPROPERTY() bool  bHasMaxHP  = false;
    UPROPERTY() int32 MaxHP      = 0;

    UPROPERTY() bool  bHasLogicState = false;
    UPROPERTY() uint8 LogicState     = 0;

    UPROPERTY() bool   bHasMoveTarget = false;
    UPROPERTY() FVector MoveTarget    = FVector::ZeroVector;
};

/**
 * 
 */
UCLASS()
class MMOARPG_API UMMOARPGGameInstance : public UGameInstance, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	virtual void Init();

	virtual void Tick(float DeltaTime);

	virtual TStatId GetStatId() const;

	virtual void Shutdown();

	static int nIndex;
public:
	void CreateClient();

	void LinkServer();
	void LinkServer(const FSimpleAddr& InAddr);
	void LinkServer(const TCHAR *InIP,uint32 InPort);

	FSimpleNetManage* GetClient();
	FMMOARPGUserData &GetUserData();
	FMMOARPGGateStatus& GetGateStatus();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TSoftObjectPtr<UDataTable> DT_Monster;
private:


	// 运行时缓存
	UPROPERTY(Transient)
	UDataTable* DT_Monster_Loaded = nullptr;

	FSimpleNetManage* Client;
	FMMOARPGUserData UserData;
	FMMOARPGGateStatus GateStatus;

public:
    /** 确保表已加载（小表可同步；大表建议在加载界面先异步预热） */
    UDataTable* EnsureMonsterTableSync();

    /** 按 MonsterId 返回行（RowName 建议即 MonsterId 字符串） */
    const FMonsterAnimRow* GetMonsterRowSync(int32 MonsterId);

    /** 同步解析成可直接使用的资源指针（若资源尚未加载，将同步加载） */
    bool GetMonsterVisualSync(int32 MonsterId, FMonsterVisualResolved& Out);

    /** 异步解析：加载完毕后回调（不阻塞） */
    void GetMonsterVisualAsync(
        int32 MonsterId,
        TFunction<void(bool bOk, const FMonsterAnimRow* Row, const FMonsterVisualResolved& Visual)> OnReady);

	 /** 异步：按 MonsterId + Pos(+Rot可选) 生成怪到当前世界 */
    void SpawnMonsterByIdAsync(int32 MonsterId, const FVector& Pos, const FRotator& Rot = FRotator::ZeroRotator);

    /** 同步（小资源/编辑器可用）：会同步加载软引用，注意别在帧中卡顿点用 */
    class AMMOARPGMonster* SpawnMonsterByIdSync(int32 MonsterId, const FVector& Pos, const FRotator& Rot = FRotator::ZeroRotator);

	// ========== 对外接口：由网络层调用 ==========
    // 8000：怪物数据（可能带位置/也可能不带）
    void GI_OnMonsterData(const FMonsterDataPacket& P, double ServerTimeMs);
    // 8300：怪物状态（Idle/Chase/Back...）
    void GI_OnMonsterState(int32 MonsterId, uint8 NewState, double ServerTimeMs);
    // 8400：怪物移动（通常自带目标位置/方向）
    void GI_OnMonsterMove(const S_MOVE_ROBOT& Move, double ServerTimeMs);

    // 供外部注册/查询（如果你有现成的，就用你自己的）
    void RegisterMonster(int32 MonsterId, AMMOARPGNetEnemyController* Ctrl);
    AMMOARPGNetEnemyController* FindMonsterCtlr(int32 MonsterId) const;
    AMMOARPGMonster*            FindMonsterById(int32 MonsterId) const;
private:
    // ========== 内部：排队与落地 ==========
    void EnqueuePending(int32 MonsterId, const FQueuedMonsterMsg& Msg);
    void OnAuthoritativeTransform(int32 MonsterId, const FTransform& T, double ServerTimeMs);
    void FlushPendingTo(AMMOARPGMonster* M, int32 MonsterId);
    void ApplyQueued(AMMOARPGMonster* M, const FQueuedMonsterMsg& Msg, bool bAuthoritative);

    // 清理超时的排队（避免堆积）
    void CleanupPending(float MaxHoldSec = 10.f);

private:
    // 未落地的队列
    TMap<int32, TArray<FQueuedMonsterMsg>> PendingMsgs;
    TMap<int32, double>                    PendingFirstSeenSec;

    // Id → 控制器（或怪物）的弱引用表
    TMap<int32, TWeakObjectPtr<AMMOARPGNetEnemyController>> IdToCtrl;
    TMap<int32, TWeakObjectPtr<AMMOARPGMonster>>            IdToMonster;

    // 清理计时器
    FTimerHandle PendingCleanupHandle;
};
