// Copyright (C) RenZhai.2021.All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "SimpleTextureHelperType.h"
#include "SimpleTextureHelperBPLibrary.generated.h"

/* The following website explains how to use this set of plug-ins
 * DocURL：			    https://zhuanlan.zhihu.com/p/372346306
 * Here's how to develop the current plug-in
 * MarketplaceURL :     https://www.aboutcg.org/courseDetails/1228/introduce
 * If you want to learn more about the UE4 tutorial, please refer to:
 * URL :				https://renzhai.net/390.html
 * If you want to learn about the series of tutorials and related plug-ins, you can follow my blog
 * URL :				http://renzhai.net/
 * If you want to know our next course arrangement,
 * you can search the WeChat public house for your attention, then we will push a lot of technical articles.
 * Sina blog            https://weibo.com/bzrz/profile?S=6cm7d0 // this blog hasn't been used for three or four years now.
 * Maybe you can re enable the first connection later
 */

UCLASS()
class SIMPLETEXTUREHELPER_API USimpleTextureHelperBPLibrary : public UObject
{
	GENERATED_UCLASS_BODY()

	//从磁盘里面读取文件转为Texture2D
	UFUNCTION(BlueprintCallable, Category = "TextureHelper")
	static UTexture2D *Uint8ArrayToTexture2DFromDisk(const FString &InLoadPath, const ESimpleImageFormat InFormat = ESimpleImageFormat::SIMPLE_JPEG, const ESimpleRGBFormat InRGBFormat = ESimpleRGBFormat::SIMPLE_BGRA, bool bTurnBlack = false);

	//将数据存储
	UFUNCTION(BlueprintCallable, Category = "TextureHelper")
	static UTexture2D* Uint8ArrayToTexture2D(const TArray<uint8> &InData,const ESimpleImageFormat InFormat = ESimpleImageFormat::SIMPLE_JPEG, const ESimpleRGBFormat InRGBFormat = ESimpleRGBFormat::SIMPLE_BGRA,bool bTurnBlack = false);

	//截屏并且存储在磁盘
	UFUNCTION(BlueprintCallable, meta = (Category = "TextureHelper", WorldContext = WorldContextObject))
	static bool ScreenShotToDisk(UObject* WorldContextObject,const FString &InSavePath, bool bShowUI = false ,const ESimpleImageFormat InFormat = ESimpleImageFormat::SIMPLE_JPEG, const ESimpleRGBFormat InRGBFormat = ESimpleRGBFormat::SIMPLE_BGRA);

	//截图 获取到实际截图RGBA数据 这个接口在编辑器里面可以截图，但是Runtime下有问题
	UFUNCTION(BlueprintCallable, meta = (Category = "TextureHelper", WorldContext = WorldContextObject))
	static FVector2D ScreenShot(UObject* WorldContextObject,TArray<FColor> &OutScreenShotData);

	//获取视口尺寸
	UFUNCTION(BlueprintCallable, meta = (Category = "TextureHelper", WorldContext = WorldContextObject))
	static FVector2D GetViewportSize(UObject* WorldContextObject);
};
