#include "Core/RenderingUtils.h"

namespace RenderingUtils
{
	FScreenShot* CreateScreenShotObject(
		int32 InWidth,
		int32 InHeight,
		FOnScreenShotCaptured InOnTestScreenshotCaptured,
		int32 InImageQuality /*= 80*/,
		bool bInShowUI /*= false*/,
		bool bAddFilenameSuffix /*= true*/)
	{
		return new FScreenShot(
			InWidth,
			InHeight,
			InOnTestScreenshotCaptured,
			InImageQuality,
			bInShowUI,
			bAddFilenameSuffix);
	}

	FScreenShot::FScreenShot(
		int32 InWidth,
		int32 InHeight,
		FOnScreenShotCaptured InOnTestScreenshotCaptured,
		int32 InImageQuality /*= 80*/,
		bool bInShowUI /*= false*/,
		bool bAddFilenameSuffix /*= true*/)
		: ScaledWidth(InWidth)
		, ScaledHeight(InHeight)
		, ImageQuality(InImageQuality)
	{
		if (!UGameViewportClient::OnScreenshotCaptured().IsBound())
		{
			ScreenShotDelegateHandle = UGameViewportClient::OnScreenshotCaptured().AddRaw(
				this,
				&RenderingUtils::FScreenShot::OnScreenshotCapturedInternal);

			FScreenshotRequest::RequestScreenshot(TEXT(""), bInShowUI, bAddFilenameSuffix);
		
			OnTestScreenshotCaptured = InOnTestScreenshotCaptured;
		}
	}

	void FScreenShot::OnScreenshotCapturedInternal(
		int32 SrcWidth,
		int32 SrcHeight,
		const TArray<FColor>& OrigBitmap)
	{
		OnTestScreenshotCaptured.ExecuteIfBound(OrigBitmap, SrcWidth, SrcHeight);

		UGameViewportClient::OnScreenshotCaptured().Remove(ScreenShotDelegateHandle);
	
		delete this;
	}

	//ASceneCapture2D* SpawnSceneCapture2D(UWorld* World, UClass* SceneCaptureClass, FMapSize& MapSize, float Life)
	//{
	//	if (SceneCaptureClass)
	//	{
	//		for (TActorIterator<ALandscape> It(World, ALandscape::StaticClass()); It; ++It)
	//		{
	//			if (ALandscape* BigMap = *It)
	//			{
	//				//都是正方形
	//				FVector BigMapSize = BigMap->GetActorScale3D();
	//				MapSize.BigMapRealSize = FVector2D(BigMapSize.X * 7, BigMapSize.Y * 7);

	//				FVector CenterPoint = FVector(MapSize.BigMapRealSize.X / 2);

	//				if (ASceneCapture2D* NewCarma = World->SpawnActor<ASceneCapture2D>(SceneCaptureClass, CenterPoint, FRotator(-90.f, 0.f, 0.f)))
	//				{
	//					if (Life != 0.f)
	//					{
	//						NewCarma->SetLifeSpan(Life);
	//					}

	//					return NewCarma;
	//				}

	//				break;
	//			}
	//		}
	//	}

	//	return nullptr;
	//}
}