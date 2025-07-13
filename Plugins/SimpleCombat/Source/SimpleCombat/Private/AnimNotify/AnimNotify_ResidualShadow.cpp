//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotify/AnimNotify_ResidualShadow.h"
#include "GameFramework/Character.h"
#include "SimpleCombatBPLibrary.h"
#include "Components/CapsuleComponent.h"

UAnimNotify_ResidualShadow::UAnimNotify_ResidualShadow()
	:ResidualShadowLifeTime(1.f)
{

}

FString UAnimNotify_ResidualShadow::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_ResidualShadow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!ResidualShadowClass)
	{
		return;
	}

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetWorld() && !InCharacter->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
		{
			USimpleCombatBPLibrary::SpawnResidualShadow(
				InCharacter->GetWorld(), ResidualShadowClass, InCharacter->GetMesh(),
				-InCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
				InCharacter->GetActorLocation(),
				InCharacter->GetActorRotation(), ResidualShadowLifeTime);
		}
	}
}