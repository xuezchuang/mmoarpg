// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SimpleNetManage.h"
#include "MMOARPGType.h"
#include "Engine/DataTable.h"
#include "MMOARPGGameInstance.generated.h"

struct FMonsterAnimRow;
struct FCharacterAnimRow;

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

	//void LinkServer();
	void LinkLoginServer();
	void LinkGateServer();

	FSimpleNetManage* GetClient();
	FMMOARPGUserData &GetUserData();
	FMMOARPGGateStatus& GetGateStatus();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TSoftObjectPtr<UDataTable> DT_Monster;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TSoftObjectPtr<UDataTable> DT_Player;
private:


	// 运行时缓存
	UPROPERTY(Transient)
	UDataTable* DT_Monster_Loaded = nullptr;

	UPROPERTY(Transient)
	UDataTable* DT_Player_Loaded = nullptr;

	FSimpleNetManage* Client;
	FMMOARPGUserData UserData;
	FMMOARPGGateStatus GateStatus;

public:
    UDataTable* EnsureMonsterTableSync();
    const FMonsterAnimRow* GetMonsterRowSync(int32 MonsterId);

	UDataTable* EnsurePlayerTableSync();
	const FCharacterAnimRow* GetPlayerRowSync(int32 JobId);

};
