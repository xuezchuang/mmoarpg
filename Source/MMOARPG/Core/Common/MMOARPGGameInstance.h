// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SimpleNetManage.h"
#include "MMOARPGType.h"
#include "Engine/DataTable.h"

#include "MMOARPGGameInstance.generated.h"

struct FMonsterAnimRow;

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
};
