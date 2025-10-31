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

    // ��������ط�
    UPROPERTY() double ServerTimeMs = 0.0;

    // �Ƿ����Ȩ��λ�ˣ��õ��󽫴������� Spawn ��Ȩ�����裩
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


	// ����ʱ����
	UPROPERTY(Transient)
	UDataTable* DT_Monster_Loaded = nullptr;

	FSimpleNetManage* Client;
	FMMOARPGUserData UserData;
	FMMOARPGGateStatus GateStatus;

public:
    /** ȷ�����Ѽ��أ�С���ͬ����������ڼ��ؽ������첽Ԥ�ȣ� */
    UDataTable* EnsureMonsterTableSync();

    /** �� MonsterId �����У�RowName ���鼴 MonsterId �ַ����� */
    const FMonsterAnimRow* GetMonsterRowSync(int32 MonsterId);

    /** ͬ�������ɿ�ֱ��ʹ�õ���Դָ�루����Դ��δ���أ���ͬ�����أ� */
    bool GetMonsterVisualSync(int32 MonsterId, FMonsterVisualResolved& Out);

    /** �첽������������Ϻ�ص����������� */
    void GetMonsterVisualAsync(
        int32 MonsterId,
        TFunction<void(bool bOk, const FMonsterAnimRow* Row, const FMonsterVisualResolved& Visual)> OnReady);

	 /** �첽���� MonsterId + Pos(+Rot��ѡ) ���ɹֵ���ǰ���� */
    void SpawnMonsterByIdAsync(int32 MonsterId, const FVector& Pos, const FRotator& Rot = FRotator::ZeroRotator);

    /** ͬ����С��Դ/�༭�����ã�����ͬ�����������ã�ע�����֡�п��ٵ��� */
    class AMMOARPGMonster* SpawnMonsterByIdSync(int32 MonsterId, const FVector& Pos, const FRotator& Rot = FRotator::ZeroRotator);

	// ========== ����ӿڣ����������� ==========
    // 8000���������ݣ����ܴ�λ��/Ҳ���ܲ�����
    void GI_OnMonsterData(const FMonsterDataPacket& P, double ServerTimeMs);
    // 8300������״̬��Idle/Chase/Back...��
    void GI_OnMonsterState(int32 MonsterId, uint8 NewState, double ServerTimeMs);
    // 8400�������ƶ���ͨ���Դ�Ŀ��λ��/����
    void GI_OnMonsterMove(const S_MOVE_ROBOT& Move, double ServerTimeMs);

    // ���ⲿע��/��ѯ����������ֳɵģ��������Լ��ģ�
    void RegisterMonster(int32 MonsterId, AMMOARPGNetEnemyController* Ctrl);
    AMMOARPGNetEnemyController* FindMonsterCtlr(int32 MonsterId) const;
    AMMOARPGMonster*            FindMonsterById(int32 MonsterId) const;
private:
    // ========== �ڲ����Ŷ������ ==========
    void EnqueuePending(int32 MonsterId, const FQueuedMonsterMsg& Msg);
    void OnAuthoritativeTransform(int32 MonsterId, const FTransform& T, double ServerTimeMs);
    void FlushPendingTo(AMMOARPGMonster* M, int32 MonsterId);
    void ApplyQueued(AMMOARPGMonster* M, const FQueuedMonsterMsg& Msg, bool bAuthoritative);

    // ����ʱ���Ŷӣ�����ѻ���
    void CleanupPending(float MaxHoldSec = 10.f);

private:
    // δ��صĶ���
    TMap<int32, TArray<FQueuedMonsterMsg>> PendingMsgs;
    TMap<int32, double>                    PendingFirstSeenSec;

    // Id �� �������������������ñ�
    TMap<int32, TWeakObjectPtr<AMMOARPGNetEnemyController>> IdToCtrl;
    TMap<int32, TWeakObjectPtr<AMMOARPGMonster>>            IdToMonster;

    // �����ʱ��
    FTimerHandle PendingCleanupHandle;
};
