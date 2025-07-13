//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AddForce.generated.h"

class ACharacter;
/**
 * 
 */
UCLASS(meta = (DisplayName = "AddForce"))
class SIMPLECOMBAT_API UAnimNotifyState_AddForce : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UAnimNotifyState_AddForce();

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState_AddForce")
	FVector DirectionForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState_AddForce")
	float ForceSize;

private:
	float TotalDurationConsuming;
	float ForceSizeConsuming;
};
