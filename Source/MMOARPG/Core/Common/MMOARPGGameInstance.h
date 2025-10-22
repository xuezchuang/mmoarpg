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
};
