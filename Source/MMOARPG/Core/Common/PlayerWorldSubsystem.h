// PlayerWorldSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MMOARPGType.h"
#include "Channel/SimpleChannel.h"
#include "PlayerWorldSubsystem.generated.h"

class UMMOARPGNetSubsystem;
class UMMOARPGGameInstance;
class ABladeIINetPlayer;               // 其它玩家代理Pawn（你说的“别的玩家”）
class AController;                     // 如有自定义 NetPlayerController 可换成它

USTRUCT()
struct FQueuedPlayerMsg
{
    GENERATED_BODY()

    UPROPERTY() double   ServerTimes = 0.0;

    UPROPERTY() bool     bHasTransform = false;
    UPROPERTY() FTransform Transform;

    UPROPERTY() bool     bHasMoveTarget = false;
    UPROPERTY() FVector  MoveTarget = FVector::ZeroVector;

    UPROPERTY() bool     bHasHP = false;
    UPROPERTY() int32    HP = 0;

    UPROPERTY() bool     bHasMP = false;
    UPROPERTY() int32    MP = 0;

    UPROPERTY() bool     bHasState = false;
    UPROPERTY() uint8    State = 0;

    // 按需继续扩展（装备外观/动作状态等）
};

/** 跟随关卡（World）管理“可见的远端玩家” */
UCLASS()
class MMOARPG_API UPlayerWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    // 生命周期
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void RecvProtocol(uint32 Proto, FSimpleChannel* Channel);

	// 对外查询
	ABladeIINetPlayer* FindPlayerById(uint32 PlayerId);
	AController*       FindPlayerCtlr(uint32 PlayerId) const;

	void GetAllOtherPlayers(TArray<ABladeIINetPlayer*>& OutPlayers) const;
	bool IsLocalPlayerId(uint32 PlayerId) const;
private:
    void BindNet();
    void UnbindNet();

    // 生成与回放
    void EnqueuePending(uint32 PlayerId, const FQueuedPlayerMsg& Msg);
    void FlushPendingTo(ABladeIINetPlayer* P, uint32 PlayerId);
    void ApplyQueued(ABladeIINetPlayer* P, const FQueuedPlayerMsg& Msg, bool bAuthoritative);
    void SpawnPlayerByJobIdSync(uint32 PlayerId, uint32 jobId, const FTransform& T, double ServerTimes);

    // 进入本地图/离开本地图
    void OnPlayerEnterMap(uint32 PlayerId, const FVector& SpawnPos, const FRotator& SpawnRot);
    void OnPlayerLeaveMap(uint32 PlayerId);

    // 同步版生成（小场景/编辑器）
	ABladeIINetPlayer* SpawnPlayerProxySync(uint32 jobId, const FVector& Pos, const FRotator& Rot);

    // 定时清理排队
    void CleanupPending(float MaxHoldSec = 10.f);

    template<class T>
    T* GetGameInstance()
    {
        return GetWorld() ? GetWorld()->GetGameInstance<T>() : nullptr;
    }

private:
    // 运行时表
    UPROPERTY(Transient)
    TMap<uint32, TObjectPtr<ABladeIINetPlayer>> IdToPlayer;

    UPROPERTY(Transient)
    TMap<uint32, TObjectPtr<AController>>       IdToCtrl;

    // 未落地排队
    TMap<uint32, TArray<FQueuedPlayerMsg>> PendingMsgs;
    TMap<uint32, double>                   PendingFirstSeenSec;

    FTimerHandle PendingCleanupHandle;

    TArray<uint32> Protos;
};
