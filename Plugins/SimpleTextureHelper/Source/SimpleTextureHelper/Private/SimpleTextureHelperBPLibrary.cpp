// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "SimpleTextureHelperBPLibrary.h"
#include "SimpleTextureHelper.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SceneViewport.h"
#include "Core/RenderingUtils.h"

USimpleTextureHelperBPLibrary::USimpleTextureHelperBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

UTexture2D *USimpleTextureHelperBPLibrary::Uint8ArrayToTexture2DFromDisk(
	const FString& InLoadPath,
	const ESimpleImageFormat InFormat, 
	const ESimpleRGBFormat InRGBFormat, 
	bool bTurnBlack)
{
	TArray<uint8> ImageData;
	FFileHelper::LoadFileToArray(ImageData,*InLoadPath);

	return Uint8ArrayToTexture2D(ImageData, InFormat, InRGBFormat, bTurnBlack);
}

UTexture2D* USimpleTextureHelperBPLibrary::Uint8ArrayToTexture2D(
	const TArray<uint8>& InData, 
	const ESimpleImageFormat InFormat, 
	const ESimpleRGBFormat InRGBFormat,
	bool bTurnBlack)
{
	UTexture2D* Texture = nullptr;
	IImageWrapperModule &ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");

	auto WrapperPtr = ImageWrapperModule.CreateImageWrapper((EImageFormat)InFormat);
	if (WrapperPtr.IsValid())
	{
		if (WrapperPtr->SetCompressed(InData.GetData(), InData.Num()))
		{
			TArray<uint8> UncompressedBGRA;
			if (WrapperPtr->GetRaw((ERGBFormat)InRGBFormat,8, UncompressedBGRA))
			{
				if (bTurnBlack)
				{
					for (int32 i = 0 ;i < UncompressedBGRA.Num() ;i+=4)
					{
						if (UncompressedBGRA[i] >= 0 && UncompressedBGRA[i] <= 5 &&
							UncompressedBGRA[i + 1] >= 0 && UncompressedBGRA[i + 1] <= 5 &&
							UncompressedBGRA[i + 2] >= 0 && UncompressedBGRA[i + 2] <= 5)
						{
							UncompressedBGRA[i] = 255 - UncompressedBGRA[i];
							UncompressedBGRA[i + 1] = 255 - UncompressedBGRA[i + 1];
							UncompressedBGRA[i + 2] = 255 - UncompressedBGRA[i + 2];
						}
					}
				}

				Texture = UTexture2D::CreateTransient(WrapperPtr->GetWidth(), WrapperPtr->GetHeight(),PF_B8G8R8A8);
			
				void *TextureAddr = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureAddr, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
				Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

				Texture->UpdateResource();
			}
		}
	}

	return Texture;
}

FVector2D USimpleTextureHelperBPLibrary::ScreenShot(UObject* WorldContextObject, TArray<FColor>& OutScreenShotData)
{
	FVector2D ViewportSize = FVector2D::ZeroVector;

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{	
		if (UGameViewportClient* InViewportClient = World->GetGameViewport())
		{
			if (FSceneViewport* InViewport = InViewportClient->GetGameViewport())
			{
				InViewportClient->GetViewportSize(ViewportSize);

				if (GetViewportScreenShot(
					InViewport->GetViewport(),
					OutScreenShotData,
					FIntRect(0, 0, ViewportSize.X, ViewportSize.Y)))
				{
					//成功截屏
				}	
			}
		}
	}

	return ViewportSize;
}

FVector2D USimpleTextureHelperBPLibrary::GetViewportSize(UObject* WorldContextObject)
{
	FVector2D ViewportSize = FVector2D::ZeroVector;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGameViewportClient* InViewportClient = World->GetGameViewport())
		{
			if (FSceneViewport* InViewport = InViewportClient->GetGameViewport())
			{
				InViewportClient->GetViewportSize(ViewportSize);
			}
		}
	}

	return ViewportSize;
}

bool USimpleTextureHelperBPLibrary::ScreenShotToDisk(
	UObject* WorldContextObject,
	const FString& InSavePath,
	bool bShowUI,
	const ESimpleImageFormat InFormat, 
	const ESimpleRGBFormat InRGBFormat)
{
	auto TextureHelperFunc = 
		[](
			const TArray<FColor> &InRawData,
			const FVector2D &InViewportSize,
			const FString& InSavePath,
			const ESimpleImageFormat InFormat,
			const ESimpleRGBFormat InRGBFormat)->bool
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

		TSharedPtr<IImageWrapper> WrapperPtr = ImageWrapperModule.CreateImageWrapper((EImageFormat)InFormat);
		WrapperPtr->SetRaw(InRawData.GetData(), InRawData.GetAllocatedSize(), InViewportSize.X, InViewportSize.Y, (ERGBFormat)InRGBFormat, 8);

		const TArray64<uint8>& IMGData = WrapperPtr->GetCompressed();

		return FFileHelper::SaveArrayToFile(IMGData, *InSavePath);
	};

#if 0
	TArray<FColor> RawData;
	FVector2D ViewportSize = ScreenShot(WorldContextObject, RawData);
	
	return TextureHelperFunc(
		RawData,
		ViewportSize,
		InSavePath,
		InFormat,
		InRGBFormat);
#else
	auto HandleLambda = FOnScreenShotCaptured::CreateLambda(
		[TextureHelperFunc, InSavePath, InFormat, InRGBFormat]
	(const TArray<FColor>& InData, int32 InX, int32 InY)
		{
			if (TextureHelperFunc(
				InData,
				FVector2D(InX, InY),
				InSavePath,
				InFormat,
				InRGBFormat))
			{

			}
		});

	FVector2D ViewportSize = GetViewportSize(WorldContextObject);
	return RenderingUtils::CreateScreenShotObject(
		ViewportSize.X,
		ViewportSize.Y,
		HandleLambda,
		90,
		bShowUI) != NULL;
#endif
}