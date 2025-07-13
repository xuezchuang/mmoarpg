// Copyright (C) RenZhai.2020.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SimpleAdvancedAnimationType.generated.h"

typedef int32 FSAAHandle;

USTRUCT(BlueprintType)
struct FFootIKInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SimpleAdvancedAnimation|FootIKInfo")
	float Offset;
};