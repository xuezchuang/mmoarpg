//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AnimSignal.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "AnimSignal"))
class SIMPLECOMBAT_API UAnimNotify_AnimSignal : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_AnimSignal();

	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	int32 SignalValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FString SignalParam;
};
