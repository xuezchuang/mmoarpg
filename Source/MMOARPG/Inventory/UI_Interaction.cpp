// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_Interaction.h"
#include "InventoryGameState.h"
#include "UMG/Public/Components/Image.h"
#include "UMG/Public/Components/TextBlock.h"

void UUI_Interaction::NativeConstruct()
{
	Super::NativeConstruct();
	//PlayAnimationForward(Animation);
	//PlayAnimationForward(LoopingPulse);
	PlayAnimation(Animation, 0.f, 0, EUMGSequencePlayMode::Forward, 1.f);
	PlayAnimation(LoopingPulse, 0.f, 0, EUMGSequencePlayMode::Forward, 1.f);
}

void UUI_Interaction::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_Interaction::SetBorderFill(float Value)
{
	//--------------------------------------------
	// 1. 停止动画 FillAnim
	//--------------------------------------------
	if (FillAnim)
	{
		StopAnimation(FillAnim);
	}

	//--------------------------------------------
	// 2. 设置 Img_FillBorder 的可见性为 Visible
	//--------------------------------------------
	if (Img_FillBorder)
	{
		Img_FillBorder->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Img_FillBorder is NULL in SetBorderFill"));
	}

	//--------------------------------------------
	// 3. 调用 SetFillDecimalValue(Value)
	//--------------------------------------------
	SetFillDecimalValue(Value);
}


void UUI_Interaction::SetFillDecimalValue(float Value)
{
	if (!Img_FillBorder)
	{
		UE_LOG(LogTemp, Warning, TEXT("Img_FillBorder is null!"));
		return;
	}

	// 1. 获取材质动态实例
	UMaterialInstanceDynamic* DynMat = Img_FillBorder->GetDynamicMaterial();
	if (!DynMat)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get dynamic material from Img_FillBorder"));
		return;
	}

	// 2. Clamp 数值（0.05 ~ 1.0）
	const float ClampedValue = FMath::Clamp(Value, 0.05f, 1.0f);

	// 3. 设置材质 scalar 参数
	DynMat->SetScalarParameterValue(TEXT("Progress"), ClampedValue);
}


