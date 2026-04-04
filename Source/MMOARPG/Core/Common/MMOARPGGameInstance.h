// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SimpleNetManage.h"
#include "MMOARPGType.h"
#include "MMOARPGNetSubsystem.h"
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
	bool CanSendGameplayProtocols() const;
	void QueueEnterWorldAfterTravel();
	void TrySendDeferredEnterWorld(UWorld* LoadedWorld);
	void HandleQuickTestMapLoaded(UWorld* LoadedWorld);
	void TryStartQuickTestBootstrap(UWorld* LoadedWorld);
	void StartQuickTestNetworkFlow(UWorld* LoadedWorld);
	void StopQuickTestNetworkFlow();
	void ResetQuickTestRuntimeState();
	void QuickTestLinkInit(ENetServerRole ServerRole);
	void RecvQuickTestProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);
	int32 ResolveQuickTestCharacterSlot() const;
	FString GetQuickTestTravelMap() const;

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
	/** 快速测试模式配置 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuickTest")
	bool bEnableQuickTest = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuickTest",
		meta = (EditCondition = "bEnableQuickTest"))
	FString QuickTestAccount = TEXT("xuezc_1");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuickTest",
		meta = (EditCondition = "bEnableQuickTest"))
	FString QuickTestPassword = TEXT("123456");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuickTest",
		meta = (EditCondition = "bEnableQuickTest"))
	int32 QuickTestCharacterSlot = 0;

private:
	FString QuickTestStartupMap;
	bool bQuickTestBootstrapping = false;
	bool bQuickTestNetworkFlowActive = false;
	bool bQuickTestLoginRequestSent = false;
	bool bQuickTestCharacterLoginSent = false;
	bool bPendingEnterWorldAfterTravel = false;
	bool bEnterWorldSentForCurrentTravel = false;

public:
    UDataTable* EnsureMonsterTableSync();
    const FMonsterAnimRow* GetMonsterRowSync(int32 MonsterId);

	UDataTable* EnsurePlayerTableSync();
	const FCharacterAnimRow* GetPlayerRowSync(int32 JobId);

};
