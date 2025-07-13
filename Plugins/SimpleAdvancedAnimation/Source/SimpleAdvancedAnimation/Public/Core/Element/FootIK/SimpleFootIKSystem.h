// Copyright (C) RenZhai.2020.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Core/Element/FootIK/SimpleFootIK.h"
#include "SimpleAdvancedAnimationType.h"

class FFootIKSystem
{
public:
	void Tick(float DeltaTime);

	//FootIK
public:
	FSimpleFootIK* FindFootIK(const FSAAHandle InKey);
	FSAAHandle CreateFootIK(ACharacter* InCharacter, const TArray<FName>& InBoneNames, float TraceDistance = 50.f, float InterpSpeed = 18.f, float InTraceStart = 50.f);
	float FindOffset(const FSAAHandle InKey, const FName& InKeyName);
	FFootIKInfo* FindFootIKInfo(const FSAAHandle InKey, const FName& InKeyName);

protected:
	TMap<FSAAHandle, FSimpleFootIK> FootIKs;
};