#include "MMOARPGNetSubsystem.h"
#include "Async/Async.h"
#include "UObject/Core/SimpleNetObject.h"
#include "ThreadManage.h"
#include "MMOARPGGameInstance.h"
#include "MMOARPGMacroType.h"
#include "Channel/SimpleChannel.h"
#include "UObject/SimpleController.h"
#include "SocketSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "MMOARPG.h"
void UMMOARPGNetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // 默认不主动绑定，给你显式控制（也可在此寻找 GI->GetClient()->GetController() 再包装）
}

void UMMOARPGNetSubsystem::Deinitialize()
{
    UnbindClientRcv();
    //ProtoToDelegates.Empty();
    //AnyPacket.Clear();
    Super::Deinitialize();
}

void UMMOARPGNetSubsystem::BindClientRcv()
{
	UnbindClientRcv();

	const int32 ThisGen = ++BindGen;

    // 尝试一次
    if (USimpleNetworkObject* Ctrl = TryGetReadyController())
	{
		CurrentRole = DesiredRole;
		if (OnNetLinked.IsBound())
		{
			OnNetLinked.Execute(CurrentRole);
		}
		

        // 推荐 AddUObject：生命周期跟随 this，避免捕获 [&] 的悬空问题
        RecvHandle = Ctrl->RecvDelegate.AddUObject(
            this, &UMMOARPGNetSubsystem::RecvProtocol);

        BoundCtrl = Ctrl;
        UE_LOG(LogTemp, Log, TEXT("[NetSubsys] BindClientRcv OK (gen=%d)"), ThisGen);
        return;
    }

    // 未就绪：延迟重试；回调里校验世代号
    GThread::Get()->GetCoroutines().BindLambda(0.5f, [this, ThisGen]()
    {
        if (ThisGen != BindGen) return; // 老回调，丢弃
        BindClientRcv();
    });
}


USimpleNetworkObject* UMMOARPGNetSubsystem::TryGetReadyController()
{
    if (UMMOARPGGameInstance* GI = Cast<UMMOARPGGameInstance>(GetGameInstance()))
    {
        if (FSimpleNetManage* Client = GI->GetClient())
        {
            if (USimpleNetworkObject* Ctrl = Client->GetController())
            {
                if (auto* Ch = Ctrl->GetChannel())
                {
                    if (TSharedPtr<FSimpleConnetion> Conn = Ch->GetConnetion())
                    {
                        if (Conn->GetState() == ESimpleConnetionLinkType::LINK_ConnectSecure)
                        {
                            return Ctrl;
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

void UMMOARPGNetSubsystem::RecvProtocol(uint32 ProtocolNumber, FSimpleChannel* Channel)
{
    // 单消费者优先：不复制 payload，直接把 Channel* 交给唯一处理者
    if (FProtocolHandler* H = UniqueHandlers.Find(ProtocolNumber))
    {
        if (H->IsBound())
        {
            (*H).Execute(ProtocolNumber, Channel);
            return; // 已消费，且只有这一个
        }
    }

	if (auto* MC = ProtoMulticast.Find(ProtocolNumber))
		MC->Broadcast(ProtocolNumber);
}

void UMMOARPGNetSubsystem::BeginLink(ENetServerRole InRole)
{
	DesiredRole = InRole;
	if (UMMOARPGGameInstance* GI = Cast<UMMOARPGGameInstance>(GetGameInstance()))
	{
		GI->CreateClient();
		if (InRole == ENetServerRole::Login)
		{
			GI->LinkLoginServer();
		}
		else if (InRole == ENetServerRole::Gate)
		{
			GI->LinkGateServer();
		}
	}
	BindClientRcv();
}

void UMMOARPGNetSubsystem::UnbindClientRcv()
{
	// 移除委托
	if (RecvHandle.IsValid())
	{
		if (USimpleNetworkObject* Ctrl = BoundCtrl.Get())
		{
			Ctrl->RecvDelegate.Remove(RecvHandle);
		}
	}
	RecvHandle.Reset();
	BoundCtrl.Reset();
}

bool UMMOARPGNetSubsystem::RegisterUniqueHandler(uint32 Proto, FProtocolHandler InHandler)
{
    if (!InHandler.IsBound())
        return false;

    if (UniqueHandlers.Contains(Proto))
    {
        // 调试期直接炸：你希望严格“一协议仅一消费者”
        checkf(false, TEXT("RegisterUniqueHandler: Proto %u already has a handler!"), Proto);
        return false;
    }

    UniqueHandlers.Add(Proto, MoveTemp(InHandler));
    return true;
}

bool UMMOARPGNetSubsystem::RegisterUniqueHandlers(const TArray<uint32>& Protos, const FProtocolHandler& InHandler)
{
    if (!InHandler.IsBound())
        return false;

    bool bAllOK = true;

    for (uint32 Proto : Protos)
    {
        if (UniqueHandlers.Contains(Proto))
        {
            UE_LOG(LogTemp, Warning, TEXT("RegisterUniqueHandlers: Proto %u already has a handler!"), Proto);
            bAllOK = false;
            continue;
        }

        UniqueHandlers.Add(Proto, InHandler); // 同一个 handler
    }

    return bAllOK;
}

bool UMMOARPGNetSubsystem::UnRegisterUniqueHandler(uint32 Proto)
{
    return UniqueHandlers.Remove(Proto) > 0;
}

bool UMMOARPGNetSubsystem::UnRegisterUniqueHandlers(const TArray<uint32>& Protos)
{
    bool bAnyRemoved = false;

    for (uint32 Proto : Protos)
    {
        if (UniqueHandlers.Remove(Proto) > 0)
        {
            bAnyRemoved = true;
        }
    }

    return bAnyRemoved;
}
