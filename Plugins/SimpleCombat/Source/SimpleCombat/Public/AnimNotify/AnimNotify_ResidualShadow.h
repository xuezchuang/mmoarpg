//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ResidualShadow.generated.h"

class AResidualShadowActor;
/**
 * 
 */
UCLASS(meta = (DisplayName = "ResidualShadow"))
class SIMPLECOMBAT_API UAnimNotify_ResidualShadow : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_ResidualShadow();

	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	TSubclassOf<AResidualShadowActor> ResidualShadowClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float ResidualShadowLifeTime;
};
