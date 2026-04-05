// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent.h"
#include "../Game/Character/Core/MMOARPGCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MMOARPGType.h"
#include "../../MMOARPG.h"
#include "../../Inventory/UI_Vendor.h"
#include "../../MMOARPGBPLibrary.h"
#include "../../Inventory/InventoryGameState.h"
#include "Inventory/UI_Interaction.h"
#include "InventoryComponent.h"

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

	if (InteractionWidget)
	{
		InteractionWidget->SetBorderFill(0.f);
	}
}

void UInteractionComponent::Interaction()
{
	if (!GetWorld())
	{
		return;
	}

	if (InteractionType == E_InTeractableType::Vendor)
	{
		if (UInventoryComponent* InventoryComponent = UInventoryComponent::GetInventoryComponent(this))
		{
			UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Interaction open shared vendor UI [SourceInteraction:%p DataTableType:%d]"),
				this,
				static_cast<int32>(DataTableType));
			InventoryComponent->ShowVendor(this);
		}
		else
		{
			UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] Interaction failed because local InventoryComponent was not found [SourceInteraction:%p DataTableType:%d]"),
				this,
				static_cast<int32>(DataTableType));
		}
		return;
	}

	if (!InteractionUIMap.Contains(InteractionType))
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionType %d not mapped to any UI!"), (int32)InteractionType);
		return;
	}

	TSubclassOf<UUserWidget> UIClass = InteractionUIMap[InteractionType];
	if (!UIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UIClass is null for InteractionType %d"), (int32)InteractionType);
		return;
	}

	UUserWidget* CreatedUI = CreateWidget<UUserWidget>(GetWorld(), UIClass);
	if (!CreatedUI)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create widget for InteractionType %d"), (int32)InteractionType);
		return;
	}

	CreatedUI->AddToViewport();

	UE_LOG(LogTemp, Log, TEXT("Opened UI for InteractionType: %d"), (int32)InteractionType);
}

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

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

	HoldProgress += DeltaTime / HoldTime;
	HoldProgress = FMath::Clamp(HoldProgress, 0.f, 1.f);

	if (InteractionWidget)
	{
		InteractionWidget->SetBorderFill(HoldProgress);
	}

	if (HoldProgress >= 1.f)
	{
		bIsHolding = false;
		HoldProgress = 0.f;

		Interaction();

		if (InteractionWidget)
		{
			InteractionWidget->SetBorderFill(0.f);
		}
	}
}

void UInteractionComponent::Initialize(UWidget* InInteractionWidget)
{
	if (!InInteractionWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Initialize_BP failed: InInteractionWidget is nullptr on %s"), *GetName());
		return;
	}

	InteractionWidget = Cast<UUI_Interaction>(InInteractionWidget);

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
