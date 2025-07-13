//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_SlowMotion.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "SlowMotion"))
class SIMPLECOMBAT_API UAnimNotifyState_SlowMotion : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_SlowMotion();

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState_SlowMotion")
	float Speed;
};
