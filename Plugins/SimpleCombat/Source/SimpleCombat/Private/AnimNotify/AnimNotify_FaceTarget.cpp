//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotify/AnimNotify_FaceTarget.h"
#include "CombatInterface/SimpleCombatInterface.h"

UAnimNotify_FaceTarget::UAnimNotify_FaceTarget()
{
	bYaw = true;
	bPitch = false;
	bRoll = false;
}

FString UAnimNotify_FaceTarget::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_FaceTarget::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (ISimpleCombatInterface *InSimpleCombatInterface = Cast<ISimpleCombatInterface>(MeshComp->GetOuter()))
	{		
		if (AActor* InTarget = InSimpleCombatInterface->GetTarget())
		{
			if (AActor* InThis = Cast<AActor>(MeshComp->GetOuter()))
			{
				FMatrix Matrix = FRotationMatrix::MakeFromX(InTarget->GetActorLocation() - InThis->GetActorLocation());
				FRotator FaceRot = Matrix.Rotator();
				
				auto ResetRotatorValue = [](bool bReset,void *InAix)
				{
					if (!bReset)
					{
						*((float*)(InAix)) = 0.f;
					}
				};

				ResetRotatorValue(bYaw, &FaceRot.Yaw);
				ResetRotatorValue(bRoll, &FaceRot.Roll);
				ResetRotatorValue(bPitch, &FaceRot.Pitch);

				InThis->SetActorRotation(FaceRot);
			}
		}
	}
}