// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOARPGMapCharacter.h"
#include "UI_EditMap.h"

void AMMOARPGMapCharacter::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	m_EditMapUI = CreateWidget<UUI_EditMap>(GetWorld(), UIEditMap_BPClass);
	if (m_EditMapUI)
	{
		m_EditMapUI->AddToViewport();
		m_EditMapUI->SetVisibility(ESlateVisibility::Hidden);
	}
#endif
}


void AMMOARPGMapCharacter::OpenMapEdit()
{
#if WITH_EDITOR
	m_EditMapUI->Active();
#endif
}