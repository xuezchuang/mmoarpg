// Copyright (C) RenZhai.2020.All Rights Reserved.
/*
下面的网址讲解了如何使用本套插件
 DocURL：			https://zhuanlan.zhihu.com/p/82195344

 下面的网址讲解如何开发当前的插件
 MarketplaceURL :   https://www.aboutcg.org/courseDetails/682/introduce

 如果想了解更多关于UE4教程请参考：
 URL :				https://zhuanlan.zhihu.com/p/60117613

 如果想系统了解人宅系列教程以及相关插件迭代更新 可以关注我的博客
 URL :				http://renzhai.net/

 如果想了解我们下一节的课程安排可以 可以在微信公众号搜所 人宅 关注即可 我们会推送很多富有技术的文章

 新浪博客            https://weibo.com/BZRZ/profile?s=6cm7D0  //这个博客从16年到现在 有三四年没怎么用过 以后说不定可以重新启用 先发个连接
 */
#pragma once

#include "CoreMinimal.h"
#include "Request/RequestInterface.h"

namespace SimpleHTTP
{
	namespace HTTP
	{
		struct FPutObjectRequest :IHTTPClientRequest
		{
			FPutObjectRequest(const FString &URL, const FString& ContentString);
			FPutObjectRequest(const FString &URL, const TArray<uint8>& ContentPayload);
			FPutObjectRequest(const FString &URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream);
		};

		struct FGetObjectRequest :IHTTPClientRequest
		{
			FGetObjectRequest(const FString &URL);
		};

		struct FDeleteObjectsRequest :IHTTPClientRequest
		{
			FDeleteObjectsRequest(const FString &URL);
		};

		struct FPostObjectsRequest :IHTTPClientRequest
		{
			FPostObjectsRequest(const FString &URL);
		};
	}
}