//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PlayGameplayEffect.generated.h"

class UGameplayEffect;
class UCustomReleasingBuffTarget;
/**
 * 
 */
UCLASS(meta = (DisplayName = "PlayGameplayEffect"))
class SIMPLECOMBAT_API UAnimNotify_PlayGameplayEffect : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_PlayGameplayEffect();

	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;
			
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	TSubclassOf<UCustomReleasingBuffTarget> CustomReleasingBuffTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	bool bTarget;
};
