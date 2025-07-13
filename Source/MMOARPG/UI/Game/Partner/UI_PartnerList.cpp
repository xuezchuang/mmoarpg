// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_PartnerList.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI_PartnerInfo.h"

void UUI_PartnerList::NativeConstruct()
{
	Super::NativeConstruct();

	Init();
}

void UUI_PartnerList::NativeDestruct()
{
	Super::NativeDestruct();


}

void UUI_PartnerList::Init()
{
	if (UI_PartnerInfoClass)
	{
		for (int32 i = 1;i < 3;i++)
		{
			if (UUI_PartnerInfo *InPartner = CreateWidget<UUI_PartnerInfo>(GetWorld(),UI_PartnerInfoClass))
			{
				if (i==1)
				{
					InPartner->ShowSelected(true);
				}

				InPartner->SetParents(this);
				if (UVerticalBoxSlot *InBoxSlot = Cast<UVerticalBoxSlot>(CharacterList->AddChildToVerticalBox(InPartner)))
				{
					InBoxSlot->SetPadding(FMargin(0.f,0.f,0.f,10.f));
					InPartner->SetCharacterID(i);
				}
			}
		}
	}	
}

void UUI_PartnerList::CallCharacterListByPredicate(TFunction<bool(UUI_PartnerInfo* Info)> InFun)
{
	for (int32 i =0;i< CharacterList->GetChildrenCount();i++)
	{
		if (UUI_PartnerInfo* InInfo = Cast<UUI_PartnerInfo>(CharacterList->GetChildAt(i)))
		{
			if (!InFun(InInfo))
			{
				break;
			}
		}
	}
}