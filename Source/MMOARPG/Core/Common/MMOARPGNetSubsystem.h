#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HAL/ThreadSafeBool.h"
#include "MMOARPGType.h"
#include "MMOARPGNetSubsystem.generated.h"

// 你的底层网络通道类型（已存在于你的工程中）
class FSimpleChannel;

class USimpleNetworkObject;

UENUM(BlueprintType)
enum class ENetServerRole : uint8
{
    Unknown = 0,
    Login,
    Gate
};


DECLARE_DELEGATE_TwoParams(FProtocolHandler, uint32 /*Proto*/, FSimpleChannel* /*Channel*/);

DECLARE_DELEGATE_OneParam(FOnNetLinked, ENetServerRole);

UCLASS()
class MMOARPG_API UMMOARPGNetSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // ---------- 生命周期 ----------
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

	bool RegisterUniqueHandler(uint32 Proto, FProtocolHandler InHandler);
	bool UnRegisterUniqueHandler(uint32 Proto);

	void BeginLink(ENetServerRole InRole);

	// 单播事件
    FOnNetLinked OnNetLinked;

    // 当前连接角色（上次绑定成功的）
    ENetServerRole CurrentRole = ENetServerRole::Unknown;


	// 手动解绑（切服/断开时）
	void UnbindClientRcv();

private:


    // 强类型解析与分发
    void RouteTyped(uint32 Protocol, const TArray<uint8>& Payload);

	void BindClientRcv();
	void RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel);

	USimpleNetworkObject* TryGetReadyController();

private:
	FDelegateHandle RecvHandle;                       // 记录已绑定的句柄
	TWeakObjectPtr<USimpleNetworkObject> BoundCtrl;   // 保存当前绑定的控制器
	int32 BindGen = 0;                                // 绑定世代，避免旧定时器回调“越时空”执行

private:

	TMap<uint32, FProtocolHandler> UniqueHandlers;


    // 协议号（示例：玩家HP）
    uint32 Proto_PlayerHP = 0;

    // 防止重复绑定
    FThreadSafeBool bBound = false;

	// 当前&待切换的角色
	ENetServerRole DesiredRole = ENetServerRole::Unknown;
};
