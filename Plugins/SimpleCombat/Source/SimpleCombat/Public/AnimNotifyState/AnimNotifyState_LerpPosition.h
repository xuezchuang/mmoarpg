//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_LerpPosition.generated.h"

class ACharacter;
/**
 * 
 */
UCLASS(meta = (DisplayName = "LerpPosition"))
class SIMPLECOMBAT_API UAnimNotifyState_LerpPosition : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_LerpPosition();

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LerpPosition")
	FVector Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LerpPosition")
	bool bAbsoluteCoordinates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LerpPosition")
	float Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LerpPosition")
	bool bPlayOnlyServer;
private:
	UPROPERTY()
	float TotalDurationConsuming;

	UPROPERTY()
	FVector EndPositionInterval;
};
