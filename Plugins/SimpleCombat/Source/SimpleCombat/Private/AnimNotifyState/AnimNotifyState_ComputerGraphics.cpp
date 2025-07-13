//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "AnimNotifyState/AnimNotifyState_ComputerGraphics.h"
#include "SimpleComboType.h"
#include "GameFramework/Character.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

#if WITH_EDITOR
#include "AnimationEditorPreviewActor.h"
#endif

UAnimNotifyState_ComputerGraphics::UAnimNotifyState_ComputerGraphics()
	:bControllerRotation(false)
	,bCameraHoming(true)
	,bPlayCameraAnim(false)
{
	FieldOfView = 90.0f;
	AspectRatio = 1.777778f;
	OrthoWidth = 512.0f;
	OrthoNearClipPlane = 0.0f;
	OrthoFarClipPlane = WORLD_MAX;
	bConstrainAspectRatio = false;
	bUseFieldOfViewForLOD = true;
	PostProcessBlendWeight = 1.0f;
	ProjectionMode = ECameraProjectionMode::Perspective;
}

void UAnimNotifyState_ComputerGraphics::NotifyBegin(
	USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation, 
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AActor *InCharacter = Cast<AActor>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetLocalRole() == ENetRole::ROLE_AutonomousProxy 
#if WITH_EDITOR
			|| Cast<AAnimationEditorPreviewActor>(MeshComp->GetOuter())
#endif
		)
		{			
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				
			auto CamaraPos = InCharacter->GetActorLocation() + RelativePos;
				
			auto NewRot = Rotation;
			if (!bControllerRotation)
			{
				auto ViewPos = CameraViewOffsetValue + InCharacter->GetActorLocation();
				NewRot = FRotationMatrix::MakeFromX(ViewPos - CamaraPos).Rotator();
			}

			if (ACameraActor *InCamera = InCharacter->GetWorld()->SpawnActor<ACameraActor>(
				ACameraActor::StaticClass(),
				CamaraPos,
				NewRot, SpawnParameters))
			{
				{
					InCamera->SetLifeSpan(TotalDuration);
					InCamera->GetCameraComponent()->SetFieldOfView(FieldOfView);
					InCamera->GetCameraComponent()->SetOrthoWidth(OrthoWidth);
					InCamera->GetCameraComponent()->SetOrthoNearClipPlane(OrthoNearClipPlane);
					InCamera->GetCameraComponent()->SetOrthoFarClipPlane(OrthoFarClipPlane);
					InCamera->GetCameraComponent()->SetAspectRatio(AspectRatio);
					InCamera->GetCameraComponent()->SetConstraintAspectRatio(bConstrainAspectRatio);
					InCamera->GetCameraComponent()->SetUseFieldOfViewForLOD(bUseFieldOfViewForLOD);
					InCamera->GetCameraComponent()->SetProjectionMode(ProjectionMode);
					InCamera->GetCameraComponent()->PostProcessSettings = PostProcessSettings;
				}
				if (ACharacter *InMyCharacter=  Cast<ACharacter>(InCharacter))
				{
					if (APlayerController* InController = Cast<APlayerController>(InMyCharacter->GetController()))
					{
						InController->SetViewTargetWithBlend(InCamera,
							bPlayCameraAnim? TotalDuration:0.f);
					}
				}
			}
		}
	}
}

void UAnimNotifyState_ComputerGraphics::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime,EventReference);


}

void UAnimNotifyState_ComputerGraphics::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);

	if (bCameraHoming)
	{
		if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
		{
			if (InCharacter->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
			{
				if (APlayerController* InController = Cast<APlayerController>(InCharacter->GetController()))
				{
					InController->SetViewTargetWithBlend(InCharacter, 0.f);
				}
			}
		}
	}
}