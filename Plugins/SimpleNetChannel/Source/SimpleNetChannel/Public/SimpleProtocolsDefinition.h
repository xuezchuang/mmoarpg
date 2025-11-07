// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Channel/SimpleChannel.h"
#include "Stream/SimpleIOStream.h"
#include "SimpleNetChannelType.h"
#include "SimpleNetManage.h"

template<uint32 ProtocolsType>
class FSimpleProtocols{};

#define DEFINITION_SIMPLE_BUFFER  \
TArray<uint8> Buffer; \
FSimpleIOStream Stream(Buffer);

#define DEFINITION_SIMPLE_HEARD(ProtocolsName) \
FSimpleBunchHead Head; \
Head.ProtocolsNumber = (uint16)SP_##ProtocolsName;\
Stream << Head;

class SIMPLENETCHANNEL_API FRecursionMessageInfo
{
public:
	template<typename ...ParamTypes>
	static int32 GetBuildParams(ParamTypes &...Param)
	{
		return sizeof...(Param);
	}

	template<typename ...ParamTypes>
	static void BuildSendParams(FSimpleIOStream& InStream, ParamTypes &...Param){}
	
	//Input parameters to the stream recursively
	template<class T,typename ...ParamTypes>
	static void BuildSendParams(FSimpleIOStream& InStream, T& FirstParam, ParamTypes &...Param)
	{
		InStream << FirstParam;
		BuildSendParams(InStream, Param...);
	}

	template<typename ...ParamTypes>
	static void BuildReceiveParams(FSimpleIOStream& InStream, ParamTypes &...Param) {}

	template<class T, typename ...ParamTypes>
	static void BuildReceiveParams(FSimpleIOStream& InStream, T& FirstParam, ParamTypes &...Param)
	{
		InStream >> FirstParam;
		BuildReceiveParams(InStream, Param...);
	}
};

#define DEFINITION_PROTOCOLS(ProtocolsName,ProtocolsNumber,bForceSend) \
enum{SP_##ProtocolsName = ProtocolsNumber}; \
template<> class SIMPLENETCHANNEL_API FSimpleProtocols<ProtocolsNumber> \
{ \
public: \
	template<typename ...ParamTypes> \
	static void Send(FSimpleChannel* InChannel,ParamTypes &...Params) \
	{ \
		DEFINITION_SIMPLE_BUFFER \
		DEFINITION_SIMPLE_HEARD(ProtocolsName) \
		FRecursionMessageInfo::BuildSendParams(Stream, Params...); \
		InChannel->Send(Buffer,bForceSend);\
	} \
	template<typename ...ParamTypes> \
	static void Receive(FSimpleChannel* InChannel,ParamTypes &...Params) \
	{ \
		DEFINITION_SIMPLE_BUFFER \
		if (InChannel->Receive(Buffer)) \
		{ \
			Stream.Seek(sizeof(FSimpleBunchHead)); \
			FRecursionMessageInfo::BuildReceiveParams(Stream,Params...); \
		} \
	} \
};

#define DEFINITION_SIMPLE_PROTOCOLS(ProtocolsName,ProtocolsNumber) DEFINITION_PROTOCOLS(ProtocolsName,ProtocolsNumber,false)
#define DEFINITION_SIMPLE_PROTOCOLS_FORCE(ProtocolsName,ProtocolsNumber) DEFINITION_PROTOCOLS(ProtocolsName,ProtocolsNumber,true)


#if/* PLATFORM_IOS ||*/ PLATFORM_ANDROID
#define SIMPLE_PROTOCOLS_SEND(InProtocols,args...) FSimpleProtocols<InProtocols>::Send(Channel,##args);
#define SIMPLE_PROTOCOLS_RECEIVE(InProtocols,args...) FSimpleProtocols<InProtocols>::Receive(Channel,##args);

//Make sure simplemannage is the connection side
#define SIMPLE_CLIENT_SEND(SimpleManage,InProtocols,args...) \
if (SimpleManage && SimpleManage->GetController()) \
{ \
	if (FSimpleChannel* SimpleChannel_SIMPLE = SimpleManage->GetChannel()) \
	{ \
		FSimpleProtocols<InProtocols>::Send(SimpleChannel_SIMPLE,##args); \
	} \
}

//Make sure simplemannage is the connection side
#define SIMPLE_CLIENT_RECV(SimpleManage,InProtocols,args...) \
if (SimpleManage && SimpleManage->GetController()) \
{ \
	if (FSimpleChannel* SimpleChannel_SIMPLE = SimpleManage->GetChannel()) \
	{ \
		FSimpleProtocols<InProtocols>::Receive(SimpleChannel_SIMPLE,##args); \
	} \
}

//To ensure simplemannage, the listening server must call our listening port through other terminals to send data to other remote terminals
#define SIMPLE_SERVER_SEND(SimpleManage,InProtocols,AddrInfo,args...) \
if (SimpleManage && SimpleManage->GetController()) \
{ \
	if (FSimpleChannel* SimpleChannel_SIMPLE = SimpleManage->GetChannel(AddrInfo.Addr.IP,AddrInfo.Addr.Port,AddrInfo.ChannelID)) \
	{ \
		FSimpleProtocols<InProtocols>::Send(SimpleChannel_SIMPLE,##args); \
	} \
}
//To ensure simplemannage, the listening server must call our listening port through other terminals to send data to other remote terminals
#define SIMPLE_SERVER_RECV(SimpleManage,InProtocols,AddrInfo,args...) \
if (SimpleManage && SimpleManage->GetController()) \
{ \
	if (FSimpleChannel* SimpleChannel_SIMPLE = SimpleManage->GetChannel(AddrInfo.IP,AddrInfo.Port,AddrInfo.ChannelID)) \
	{ \
		FSimpleProtocols<InProtocols>::Receive(SimpleChannel_SIMPLE,##args); \
	} \
}
#else
#define SIMPLE_PROTOCOLS_SEND(InProtocols,...) FSimpleProtocols<InProtocols>::Send(Channel,__VA_ARGS__);
#define SIMPLE_PROTOCOLS_RECEIVE(InProtocols,...) FSimpleProtocols<InProtocols>::Receive(Channel,__VA_ARGS__);

//Make sure simplemannage is the connection side
#define SIMPLE_CLIENT_SEND(SimpleManage,InProtocols,...) \
if (SimpleManage && SimpleManage->GetController()) \
{ \
	if (FSimpleChannel* SimpleChannel_SIMPLE = SimpleManage->GetChannel()) \
	{ \
		FSimpleProtocols<InProtocols>::Send(SimpleChannel_SIMPLE,__VA_ARGS__); \
	} \
}

//Make sure simplemannage is the connection side
#define SIMPLE_CLIENT_RECV(SimpleManage,InProtocols,...) \
if (SimpleManage && SimpleManage->GetController()) \
{ \
	if (FSimpleChannel* SimpleChannel_SIMPLE = SimpleManage->GetChannel()) \
	{ \
		FSimpleProtocols<InProtocols>::Receive(SimpleChannel_SIMPLE,__VA_ARGS__); \
	} \
}

//To ensure simplemannage, the listening server must call our listening port through other terminals to send data to other remote terminals
#define SIMPLE_SERVER_SEND(SimpleManage,InProtocols,AddrInfo,...) \
if (SimpleManage && SimpleManage->GetController()) \
{ \
	if (FSimpleChannel* SimpleChannel_SIMPLE = SimpleManage->GetChannel(AddrInfo.Addr.IP,AddrInfo.Addr.Port,AddrInfo.ChannelID)) \
	{ \
		FSimpleProtocols<InProtocols>::Send(SimpleChannel_SIMPLE,__VA_ARGS__); \
	} \
}
//To ensure simplemannage, the listening server must call our listening port through other terminals to send data to other remote terminals
#define SIMPLE_SERVER_RECV(SimpleManage,InProtocols,AddrInfo,...) \
if (SimpleManage && SimpleManage->GetController()) \
{ \
	if (FSimpleChannel* SimpleChannel_SIMPLE = SimpleManage->GetChannel(AddrInfo.IP,AddrInfo.Port,AddrInfo.ChannelID)) \
	{ \
		FSimpleProtocols<InProtocols>::Receive(SimpleChannel_SIMPLE,__VA_ARGS__); \
	} \
}
#endif // PLATFORM_WINDOWS

//SIMPLE_PROTOCOLS_SEND(MMM_Test, 12, TEXT("Hello"), 10.f);
//SIMPLE_PROTOCOLS_SEND(SP_Hello, TEXT("asdasd"), 12, 25);

// === Lightweight helpers for variable-length payloads (lambda style) ===

class FSimpleProtoUtil
{
public:
    // 发送：由调用方用 builder 往 Stream 里推字段（可变长）
    template<uint32 InProtocols>
    static void SendWith(FSimpleChannel* InChannel, TFunctionRef<void(FSimpleIOStream&)> Builder, bool bForceSend=false)
    {
        TArray<uint8> Buffer;
        FSimpleIOStream Stream(Buffer);

        // 写包头
        FSimpleBunchHead Head;
        Head.ProtocolsNumber = (uint16)InProtocols;
        Stream << Head;

        // （可选）写入长度占位，便于日志/跳读；如果你的协议没有长度字段，可删除这一段
        const int32 LenPos = Stream.Tell();
        uint32 PayloadLen = 0;
        Stream << PayloadLen;                  // 先占 4 字节

        const int32 PayloadStart = Stream.Tell();
        // 交给调用方写任意字段（childcmd / 变长数组 / 字符串等）
        Builder(Stream);
        const int32 PayloadEnd = Stream.Tell();

        // 回填真实长度
        PayloadLen = (uint32)(PayloadEnd - PayloadStart);
        // 覆写 Buffer 中的长度占位
        FMemory::Memcpy(Buffer.GetData() + LenPos, &PayloadLen, sizeof(uint32));

        InChannel->Send(Buffer, bForceSend);
    }

    // 接收：拿到整包，自动跳过包头，把剩余流交给 parser 处理（可变长）
    template<uint32 InProtocols>
    static bool ReceiveWith(FSimpleChannel* InChannel, TFunctionRef<void(FSimpleIOStream&)> Parser)
    {
        TArray<uint8> Buffer;
        if (!InChannel->Receive(Buffer))
        {
            return false;
        }

        FSimpleIOStream Stream(Buffer);

        // 跳过包头
        Stream.Seek(sizeof(FSimpleBunchHead));

        // 如果你在发送时写了 PayloadLen，这里读一下然后把读指针停在负载开始处
        // 若你的协议没这个字段，可以注释掉这两行
        uint32 PayloadLen = 0;
        Stream >> PayloadLen;

        Parser(Stream);   // 让调用方自由解析 childcmd / 任意字段
        return true;
    }
};

// —— 语法糖宏：和你现有的 SIMPLE_PROTOCOLS_SEND/RECEIVE 一样的调用风格 ——

// 发送（可变长）：SIMPLE_PROTOCOLS_SEND_WITH(SP_XXX, [&](FSimpleIOStream& S){ S<<child<<pos; ...; });
#define SIMPLE_PROTOCOLS_SEND_WITH(InProtocols, BuilderLambda) \
    FSimpleProtoUtil::SendWith<InProtocols>(Channel, BuilderLambda, false)

// 强制发送（立刻 flush）
#define SIMPLE_PROTOCOLS_SEND_WITH_FORCE(InProtocols, BuilderLambda) \
    FSimpleProtoUtil::SendWith<InProtocols>(Channel, BuilderLambda, true)

// 接收（可变长）：SIMPLE_PROTOCOLS_RECEIVE_WITH(SP_XXX, [&](FSimpleIOStream& S){ uint8 c; S>>c; ...; });
#define SIMPLE_PROTOCOLS_RECEIVE_WITH(InProtocols, ParserLambda) \
    FSimpleProtoUtil::ReceiveWith<InProtocols>(Channel, ParserLambda)
