//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_CheckAndStopMontage.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "CheckAndStopMontage"))
class SIMPLECOMBAT_API UAnimNotifyState_CASMontage : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_CASMontage();

public:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference);

private:
	UPROPERTY()
	FVector LastInputVector;
};

