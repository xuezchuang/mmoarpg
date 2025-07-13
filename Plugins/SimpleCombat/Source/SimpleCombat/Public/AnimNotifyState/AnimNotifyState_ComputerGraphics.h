//Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ComputerGraphics.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "ComputerGraphics"))
class SIMPLECOMBAT_API UAnimNotifyState_ComputerGraphics : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_ComputerGraphics();

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComputerGraphics")
	FVector CameraViewOffsetValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComputerGraphics")
	FVector RelativePos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComputerGraphics")
	bool bControllerRotation;

	/** The near plane distance of the orthographic view (in world units) */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	float OrthoNearClipPlane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComputerGraphics", meta = (EditCondition = "bControllerRotation"))
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComputerGraphics")
	bool bCameraHoming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComputerGraphics")
	bool bPlayCameraAnim;

	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings, meta = (UIMin = "5.0", UIMax = "170", ClampMin = "0.001", ClampMax = "360.0", Units = deg))
	float FieldOfView;

	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	float OrthoWidth;

	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	float OrthoFarClipPlane;

	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings, meta = (ClampMin = "0.1", ClampMax = "100.0", EditCondition = "bConstrainAspectRatio"))
	float AspectRatio;

	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraOptions)
	uint8 bConstrainAspectRatio : 1;

	UPROPERTY(Interp, EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = CameraOptions)
	uint8 bUseFieldOfViewForLOD : 1;

	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	TEnumAsByte<ECameraProjectionMode::Type> ProjectionMode;

	/** Indicates if PostProcessSettings should be used when using this Camera to view through. */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = PostProcess, meta = (UIMin = "0.0", UIMax = "1.0"))
	float PostProcessBlendWeight;

	UPROPERTY(Interp, BlueprintReadWrite, Category = PostProcess, meta = (ShowOnlyInnerProperties))
	struct FPostProcessSettings PostProcessSettings;
};
