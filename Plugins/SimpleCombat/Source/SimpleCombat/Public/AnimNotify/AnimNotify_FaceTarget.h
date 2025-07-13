//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FaceTarget.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "FaceTarget"))
class SIMPLECOMBAT_API UAnimNotify_FaceTarget : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_FaceTarget();

	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face Target")
	bool bYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face Target")
	bool bPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face Target")
	bool bRoll;
};
