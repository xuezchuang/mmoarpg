//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_StopRotation.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "StopRotation"))
class SIMPLECOMBAT_API UAnimNotifyState_StopRotation : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_StopRotation();

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference);
private:
	void ResetCharacter(USkeletalMeshComponent* MeshComp);
};
