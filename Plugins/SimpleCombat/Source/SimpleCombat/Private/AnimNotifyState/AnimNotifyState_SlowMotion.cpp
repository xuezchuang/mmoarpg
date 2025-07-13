//Copyright (C) RenZhai.2022.All Rights Reserved.

#include "AnimNotifyState/AnimNotifyState_SlowMotion.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UAnimNotifyState_SlowMotion::UAnimNotifyState_SlowMotion()
	:Super()
	,Speed(0.2f)
{

}

void UAnimNotifyState_SlowMotion::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetWorld() && !InCharacter->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(),Speed);
		}
	}
}

void UAnimNotifyState_SlowMotion::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);
}

void UAnimNotifyState_SlowMotion::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);

	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetWorld() && !InCharacter->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
		}
	}
}