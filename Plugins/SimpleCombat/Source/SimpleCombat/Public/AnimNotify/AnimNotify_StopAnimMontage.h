//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_StopAnimMontage.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "StopAnimMontage"))
class SIMPLECOMBAT_API UAnimNotify_StopAnimMontage : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_StopAnimMontage();

	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;

};
