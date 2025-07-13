//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_PlayTurnMontage.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "PlayTurnMontage"))
class SIMPLECOMBAT_API UAnimNotifyState_PlayTurnMontage : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_PlayTurnMontage();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	UAnimMontage* TurnMontage;
public:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference);

private:
	UPROPERTY()
	FVector LastInputVector;

	UPROPERTY()
	bool bPlayTurnMontage;
};

