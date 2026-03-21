// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent.h"
#include "../Game/Character/Core/MMOARPGCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MMOARPGType.h"
#include "../../Inventory/UI_Vendor.h"
#include "../../MMOARPGBPLibrary.h"
#include "../../Inventory/InventoryGameState.h"
#include "Inventory/UI_Interaction.h"

void UInteractionComponent::InitializeComponent()
{
	/*m_InventoryGameState = GetWorld()->GetGameState<AInventoryGameState>();*/
}

void UInteractionComponent::StartHold()
{
	bIsHolding = true;
	HoldProgress = 0.f;
}

void UInteractionComponent::StopHold()
{
	bIsHolding = false;
	HoldProgress = 0.f;

	// 重置 UI
	if (InteractionWidget)
	{
		InteractionWidget->SetBorderFill(0.f);
	}
}

void UInteractionComponent::Interaction()
{
	if (!GetWorld())
		return;

	// 检查当前 InteractionType 是否在 Map 里
	if (!InteractionUIMap.Contains(InteractionType))
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionType %d not mapped to any UI!"), (int32)InteractionType);
		return;
	}

	// 获取 UI 类
	TSubclassOf<UUserWidget> UIClass = InteractionUIMap[InteractionType];
	if (!UIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UIClass is null for InteractionType %d"), (int32)InteractionType);
		return;
	}

	// 创建 UI
	UUserWidget* CreatedUI = CreateWidget<UUserWidget>(GetWorld(), UIClass);
	if (!CreatedUI)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create widget for InteractionType %d"), (int32)InteractionType);
		return;
	}

	// 显示在屏幕
	CreatedUI->AddToViewport();

	UE_LOG(LogTemp, Log, TEXT("Opened UI for InteractionType: %d"), (int32)InteractionType);
}


// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UInteractionComponent::Print(float InTime,const FString& InString)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, InTime, FColor::Red, FString::Printf(TEXT("%s"), *InString));
	}
}


void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsHolding)
		return;

	// 累加进度
	HoldProgress += DeltaTime / HoldTime;
	HoldProgress = FMath::Clamp(HoldProgress, 0.f, 1.f);

	// 更新 UI
	if (InteractionWidget)
	{
		InteractionWidget->SetBorderFill(HoldProgress);
	}

	// 满 100%
	if (HoldProgress >= 1.f)
	{
		bIsHolding = false;      // 停止计数
		HoldProgress = 0.f;      // 重置

		Interaction();           // 触发 C++ 交互逻辑

		// 重置 UI
		if (InteractionWidget)
		{
			InteractionWidget->SetBorderFill(0.f);
		}
	}
}


void UInteractionComponent::Initialize(UWidget* InInteractionWidget)
{
	// 1. 空指针检查
	if (!InInteractionWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Initialize_BP failed: InInteractionWidget is nullptr on %s"), *GetName());
		return;
	}

	// 2. 尝试转换成你的子类 UUI_Interaction
	InteractionWidget = Cast<UUI_Interaction>(InInteractionWidget);

	// 3. 转换失败打印错误
	if (!InteractionWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Initialize_BP failed: Widget '%s' is NOT UUI_Interaction on %s"),
			*InInteractionWidget->GetName(), *GetName());
		return;
	}
}

void UInteractionComponent::ShowTitle(bool Visibility)
{
	if (!InteractionWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("InteractionWidget NOT UUI_Interaction on"));
		return;
	}

	if(Visibility)
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	else
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
}

