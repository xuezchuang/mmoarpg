// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOARPGInventoryCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "FItemData.h"
#include "UI_UpperUIBar.h"
#include "InventoryHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI_EditMap.h"

AMMOARPGInventoryCharacter::AMMOARPGInventoryCharacter()
{
	m_UpperUI = NULL;
}

void AMMOARPGInventoryCharacter::OpenInventory()
{
	//UGameplayStatics::SetGamePaused(GetWord());
	m_UpperUI->ShowWidget(E_UIType::Character);
}

void AMMOARPGInventoryCharacter::OpenQuest()
{
	//UGameplayStatics::SetGamePaused(GetWord());
	m_UpperUI->ShowWidget(E_UIType::Joural);

}

void AMMOARPGInventoryCharacter::OpenCrafting()
{
	m_UpperUI->ShowWidget(E_UIType::Crafting);
}

//void AMMOARPGInventoryCharacter::OpenMapEdit()
//{
//#if WITH_EDITOR
//	m_EditMapUI->Active();
//#endif
//}

void AMMOARPGInventoryCharacter::BeginPlay()
{
	Super::BeginPlay();
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Test2")));
	m_PlayerController = Cast<APlayerController>(GetController());
	if (m_PlayerController)
	{
		AInventoryHUD* CurrentHUD = Cast<AInventoryHUD>(m_PlayerController->GetHUD());
		if (CurrentHUD)
		{
			m_UpperUI = CurrentHUD->GetWidget();
		}
	}
//#if WITH_EDITOR
//	m_EditMapUI = CreateWidget<UUI_EditMap>(GetWorld(), UIEditMap_BPClass);
//	m_EditMapUI->AddToViewport();
//	m_EditMapUI->SetVisibility(ESlateVisibility::Hidden);
//#endif
}
