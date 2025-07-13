// Copyright (C) RenZhai.2022.All Rights Reserved.

#include "SimpleOpenSSLBPLibrary.h"
#include "SimpleOpenSSL.h"
#include "../Plugins/Editor/CryptoKeys/Source/CryptoKeysOpenSSL/Public/CryptoKeysOpenSSL.h"

USimpleOpenSSLBPLibrary::USimpleOpenSSLBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void RemoveDescriptionQuantityHeader(TArray<uint8>& InArray)
{
	int32 Number = *(int32*)InArray.GetData();
	int32 HeadSize = InArray.Num() - Number;

	for (int32 i = 0; i < HeadSize; i++)
	{
		InArray.RemoveAt(0);
	}

	//清除空内存
	InArray.Shrink();
}

void AddDescriptionQuantityHeader(TArray<uint8>& InArray)
{
	//准备头数据
	int32 Num = InArray.Num();
	int32 Total = sizeof(int32) + InArray.Num();
	int32 Remain = 0;

	//考虑数据对齐问题
	if (Total % 16)
	{
		float Value = (float)Total / 16.f;
		int32 Ceil = FMath::CeilToInt(Value);

		int32 CeilTotal = Ceil * 16;
		Remain = CeilTotal - Total;
	}

	//对数据重新调整
	TArray<uint8> ArrayData;
	ArrayData.AddUninitialized(Total + Remain);
	FMemory::Memzero(ArrayData.GetData(), ArrayData.Num());

	int32 Offset = sizeof(int32);
	FMemory::Memcpy(ArrayData.GetData(), &Num, sizeof(int32));

	Offset += Remain;
	FMemory::Memcpy(&ArrayData[Offset], InArray.GetData(), InArray.Num());

	InArray = ArrayData;
}

bool USimpleOpenSSLBPLibrary::GenerateRSAKeyUint8(TArray<uint8>& InPublicExponent, TArray<uint8>& InPrivateExponent, TArray<uint8>& InModulus)
{
	return CryptoKeysOpenSSL::GenerateNewSigningKey(InPublicExponent, InPrivateExponent, InModulus);
}

bool USimpleOpenSSLBPLibrary::GenerateRSAKeyString(FString& InPublicExponent, FString& InPrivateExponent, FString& InModulus)
{
	TArray<uint8> PublicExponent;
	TArray<uint8> PrivateExponent;
	TArray<uint8> Modulus;
	if (GenerateRSAKeyUint8(PublicExponent, PrivateExponent, Modulus))
	{
		InPublicExponent = FBase64::Encode(PublicExponent);
		InPrivateExponent = FBase64::Encode(PrivateExponent);
		InModulus = FBase64::Encode(Modulus);

		return true;
	}

	return false;
}

bool USimpleOpenSSLBPLibrary::RSAKeyToUint8(
	const FString& InPublicExponentString,
	const FString& InPrivateExponentString, 
	const FString& InModulusString, 
	TArray<uint8>& OutPublicExponent,
	TArray<uint8>& OutPrivateExponent,
	TArray<uint8>& OutModulus)
{
	return	FBase64::Decode(InPublicExponentString,		OutPublicExponent)	&&
			FBase64::Decode(InPrivateExponentString,	OutPrivateExponent) &&
			FBase64::Decode(InModulusString,			OutModulus);
}

void USimpleOpenSSLBPLibrary::EncryptDataAES_SHA1(const FString& InPassword, TArray<uint8>& InArray)
{
	//添加头部描述
	AddDescriptionQuantityHeader(InArray);

///////////////////////////////////////////////////////////////////
	//开始加密
	FSHAHash Hash;

	//算出SHA1
	FSHA1 SHA1;
	SHA1.HashBuffer(TCHAR_TO_ANSI(*InPassword), InPassword.Len(), Hash.Hash);

	//算出Key
	ANSICHAR* InKey = TCHAR_TO_UTF8(*Hash.ToString());

	//加密
	FAES::EncryptData(InArray.GetData(), InArray.Num(), InKey);
}

void USimpleOpenSSLBPLibrary::DecryptDataAES_SHA1(const FString& InPassword, TArray<uint8>& InArray)
{
	FSHAHash Hash;

	FSHA1 SHA11;
	SHA11.HashBuffer(TCHAR_TO_ANSI(*InPassword), InPassword.Len(), Hash.Hash);

	//密钥key
	ANSICHAR* InKey = TCHAR_TO_UTF8(*Hash.ToString());

	FAES::DecryptData(InArray.GetData(), InArray.Num(), InKey);

	//清除头部描述
	RemoveDescriptionQuantityHeader(InArray);
}

void USimpleOpenSSLBPLibrary::EncryptDataAES_ANSI(FGuid& InPassword, TArray<uint8>& InArray)
{
	//添加头部描述
	AddDescriptionQuantityHeader(InArray);

	FGuid Guid = FGuid::NewGuid();
	ANSICHAR* InKey = TCHAR_TO_UTF8(*Guid.ToString());
	FAES::EncryptData(InArray.GetData(), InArray.Num(), InKey);

	InPassword = Guid;
}

void USimpleOpenSSLBPLibrary::DecryptDataAES_ANSI(const FGuid& InPassword, TArray<uint8>& InArray)
{
	ANSICHAR* InKey = TCHAR_TO_UTF8(*InPassword.ToString());

	FAES::DecryptData(InArray.GetData(), InArray.Num(), InKey);

	//清除头部描述
	RemoveDescriptionQuantityHeader(InArray);
}

void USimpleOpenSSLBPLibrary::EncryptDataAES_ANSIString(FString& InPassword, TArray<uint8>& InArray)
{
	FGuid Guid;
	EncryptDataAES_ANSI(Guid, InArray);

	InPassword = Guid.ToString();
}

void USimpleOpenSSLBPLibrary::DecryptDataAES_ANSIString(const FString& InPassword, TArray<uint8>& InArray)
{
	FGuid Guid;
	if (FGuid::Parse(InPassword, Guid))
	{
		DecryptDataAES_ANSI(Guid, InArray);
	}
}


