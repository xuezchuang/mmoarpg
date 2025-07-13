// Copyright (C) RenZhai.2022.All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimpleOpenSSLBPLibrary.generated.h"

UCLASS()
class SIMPLEOPENSSL_API USimpleOpenSSLBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	//自定义密码加密 SHA1 AES 对称加密
	UFUNCTION(BlueprintCallable, Category = "SimpleOpenSSLTesting")
	static bool GenerateRSAKeyUint8(TArray<uint8> &InPublicExponent, TArray<uint8>& InPrivateExponent, TArray<uint8>& InModulus);

	UFUNCTION(BlueprintCallable, Category = "SimpleOpenSSLTesting")
	static bool GenerateRSAKeyString(FString& InPublicExponent, FString& InPrivateExponent, FString& InModulus);

	UFUNCTION(BlueprintCallable, Category = "SimpleOpenSSLTesting")
	static bool RSAKeyToUint8(const FString& InPublicExponentString, const FString& InPrivateExponentString, const FString& InModulusString, TArray<uint8>& InPublicExponent, TArray<uint8>& InPrivateExponent, TArray<uint8>& InModulus);

	////自定义密码加密 SHA1 AES 对称加密
	//UFUNCTION(BlueprintCallable, Category = "SimpleOpenSSLTesting")
	//static void EncryptDataRSA(const TArray<uint8>& PublicExponent, TArray<uint8> Modulus,TArray<uint8>& InArray);
	//
	////自定义密码加密 SHA1 AES 对称解密
	//UFUNCTION(BlueprintCallable, Category = "SimpleOpenSSLTesting")
	//static void DecryptDataRSA(const FString& InPassword, TArray<uint8>& InArray);

	//自定义密码加密 SHA1 AES 对称加密
	UFUNCTION(BlueprintCallable, Category = "SimpleOpenSSLTesting")
	static void EncryptDataAES_SHA1(const FString &InPassword, TArray<uint8> &InArray);

	//自定义密码加密 SHA1 AES 对称解密
	UFUNCTION(BlueprintCallable, Category = "SimpleOpenSSLTesting")
	static void DecryptDataAES_SHA1(const FString& InPassword, TArray<uint8>& InArray);

	//自动加密，会返回密钥
	UFUNCTION(BlueprintCallable, Category = "SimpleOpenSSLTesting")
	static void EncryptDataAES_ANSI(FGuid &InPassword, TArray<uint8> &InArray);

	//需要guid密钥
	UFUNCTION(BlueprintCallable, Category = "SimpleOpenSSLTesting")
	static void DecryptDataAES_ANSI(const FGuid& InPassword, TArray<uint8>& InArray);

	//自动加密，会返回密钥
	UFUNCTION(BlueprintCallable, Category = "SimpleOpenSSLTesting")
	static void EncryptDataAES_ANSIString(FString &InPassword, TArray<uint8> &InArray);

	//需要guid密钥
	UFUNCTION(BlueprintCallable, Category = "SimpleOpenSSLTesting")
	static void DecryptDataAES_ANSIString(const FString& InPassword, TArray<uint8>& InArray);
};
