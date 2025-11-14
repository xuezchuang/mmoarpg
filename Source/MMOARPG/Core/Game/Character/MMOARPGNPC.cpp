// Fill out your copyright notice in the Description page of Project Settings.


#include "MMOARPGNPC.h"
#include "Component/SelectableComponent.h"

AMMOARPGNPC::AMMOARPGNPC()
{
	SelectableComp = CreateDefaultSubobject<USelectableComponent>(TEXT("SelectableComp"));
}

void AMMOARPGNPC::BeginPlay()
{
	Super::BeginPlay();

	if (SelectableComp)
	{
		SelectableComp->OnSelectedEvent.AddDynamic(this, &AMMOARPGNPC::HandleSelected);
		SelectableComp->OnSelectionEndEvent.AddDynamic(this, &AMMOARPGNPC::HandleSelectionEnd);
	}
}

void AMMOARPGNPC::HandleSelected_Implementation()
{

}

void AMMOARPGNPC::HandleSelectionEnd_Implementation()
{

}
