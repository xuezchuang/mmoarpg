//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotifyState/AnimNotifyState_AddForce.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SimpleComboType.h"

UAnimNotifyState_AddForce::UAnimNotifyState_AddForce()
	:Super()
	,DirectionForce(1.f,0.f,0.f)
	,ForceSize(1800000.f)
	,TotalDurationConsuming(0.f)
	,ForceSizeConsuming(0.f)
{

}

void UAnimNotifyState_AddForce::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	TotalDurationConsuming = TotalDuration;
	ForceSizeConsuming = ForceSize;
}

void UAnimNotifyState_AddForce::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);

	if (TotalDurationConsuming > 0.f)
	{
		if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
		{
			FVector NewDirection = SimpleComboType::GetCurrentCharacterDirection(InCharacter, DirectionForce);

			InCharacter->GetCharacterMovement()->AddForce(ForceSizeConsuming * NewDirection);

			if (ForceSizeConsuming > 0.f)
			{
				//每秒帧数
				float PreSecondFrame = 1.f / FrameDeltaTime;
				//这段时间的总消耗帧数
				float TotalFrame = TotalDurationConsuming * PreSecondFrame;
				//每帧应该减去多少力
				float ForceAttenuationFrameDeltaTime = ForceSize / TotalFrame;

				//对我们的力进行衰减
				ForceSizeConsuming -= ForceAttenuationFrameDeltaTime;
			}	
		}
	}
}

void UAnimNotifyState_AddForce::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);
}

