// Copyright (C) RenZhai.2021.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SimpleTextureHelperType.generated.h"

UENUM(BlueprintType)
enum ESimpleImageFormat 
{
	SIMPLE_PNG					UMETA(DisplayName = "Png"),
	SIMPLE_JPEG					UMETA(DisplayName = "Jpeg"),
	SIMPLE_GRAYSCALEJPEG		UMETA(DisplayName = "Gray Scale Jpeg"),
	SIMPLE_BMP					UMETA(DisplayName = "Bmp"),
	SIMPLE_ICO					UMETA(DisplayName = "Ico"),
	SIMPLE_EXR					UMETA(DisplayName = "Exr"),
	SIMPLE_ICNS					UMETA(DisplayName = "Icns"),
};

UENUM(BlueprintType)
enum ESimpleRGBFormat 
{
	SIMPLE_RGBA UMETA(DisplayName = "RGBA"),
	SIMPLE_BGRA UMETA(DisplayName = "BGRA"),
	SIMPLE_GRAY UMETA(DisplayName = "Gray"),
};
