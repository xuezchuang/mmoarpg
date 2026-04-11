// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
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
#include "../Game/MMOARPGGameState.h"
#include "InteractionComponent.h"

void UInventoryComponent::InitializeComponent()
{
}

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

void UInventoryComponent::ShowVendor(UInteractionComponent* SourceInteraction)
{
	if (!m_UIShop)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] ShowVendor failed because m_UIShop is null [SourceInteraction:%p]"),
			SourceInteraction);
		return;
	}

	if (!SourceInteraction)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] ShowVendor failed because SourceInteraction is null"));
		return;
	}

	UE_LOG(MMOARPG, Display, TEXT("[VendorUI] ShowVendor [ShopUI:%p SourceInteraction:%p DataTableType:%d]"),
		m_UIShop,
		SourceInteraction,
		static_cast<int32>(SourceInteraction->DataTableType));

	m_UIShop->OpenWithInteraction(SourceInteraction);
	UMMOARPGBPLibrary::MMOARPG_InputMode(GetWorld(), EMMOARPG_InputMode::Input_UI, m_UIShop);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	m_InventoryGameState = GetWorld()->GetGameState<AMMOARPGGameState>();

	MMOARPGCharacterBase = Cast<AMMOARPGCharacterBase>(GetOwner());
	if (MMOARPGCharacterBase.IsValid())
	{
		m_UIShop = CreateWidget<UUI_Vendor>(GetWorld(), Vendor_BPClass);
		if (m_UIShop)
		{
			m_UIShop->AddToViewport();
			m_UIShop->SetVisibility(ESlateVisibility::Hidden);
			UE_LOG(MMOARPG, Display, TEXT("[VendorUI] Create shared player vendor UI [ShopUI:%p Class:%s]"),
				m_UIShop,
				*GetNameSafe(Vendor_BPClass.Get()));
		}
		else
		{
			UE_LOG(MMOARPG, Warning, TEXT("[VendorUI] Failed to create shared player vendor UI [Class:%s]"),
				*GetNameSafe(Vendor_BPClass.Get()));
		}
	}
}

void UInventoryComponent::Print(float InTime,const FString& InString)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, InTime, FColor::Red, FString::Printf(TEXT("%s"), *InString));
	}
}

void UInventoryComponent::Reset()
{
	//CharacterMovementComponent->bOrientRotationToMovement = true;
	//CharacterMovementComponent->MaxFlySpeed = 600.f;
	//CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);

	//FRotator NewRot = MMOARPGCharacterBase->GetActorRotation();
	//NewRot.Pitch = 0.f;
	//MMOARPGCharacterBase->SetActorRotation(NewRot);
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FSlateColor UInventoryComponent::GetRarityColor(E_ItemRarity rarity)
{
	if(!m_InventoryGameState)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("m_InventoryGameState = NULL"));
		return FSlateColor();
	}
	return m_InventoryGameState->GetRarityColor(rarity);
}

UTexture2D* UInventoryComponent::GetStatTexture(E_StatCategory stat)
{
	if (UTexture2D* const* Found = mapStatTexture.Find(stat))
	{
		return *Found;
	}
	return nullptr;
}

UInventoryComponent* UInventoryComponent::GetInventoryComponent(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if(!World)
	{
		return nullptr;
	}

	APlayerController* PC = World->GetFirstPlayerController<APlayerController>();
	if (!PC)
	{
		return nullptr;
	}

	ACharacter* Pawn = Cast<ACharacter>(PC->GetPawn());
	if (!Pawn)
	{
		return nullptr;
	}

	return Pawn->FindComponentByClass<UInventoryComponent>();
}

