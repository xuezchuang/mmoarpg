// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_Base.h"

UWidgetAnimation* UUI_Base::GetNameWidgetAnimation(const FString& InWidgetAnimName)
{
	if (UWidgetBlueprintGeneratedClass* WidgetBlueprintGenerated = Cast<UWidgetBlueprintGeneratedClass>(GetClass()))
	{
		TArray<UWidgetAnimation*> TArrayAnimations = WidgetBlueprintGenerated->Animations;

		UWidgetAnimation** MyTempAnimation = TArrayAnimations.FindByPredicate(
			[&](const UWidgetAnimation* OurAnimation)
			{
				return OurAnimation->GetFName().ToString() == (InWidgetAnimName + FString("_INST"));
			});

		return *MyTempAnimation;
	}

	return NULL;
}

void UUI_Base::PlayWidgetAnim(const FString& InWidgetAnimName, EUMGSequencePlayMode::Type PlayMode /*= EUMGSequencePlayMode::Forward*/, int NumLoopsToPlay /*= 1*/)
{
	if(UWidgetAnimation* MyTempAnimation = GetNameWidgetAnimation(InWidgetAnimName))
	{
		PlayAnimation(MyTempAnimation, 0.0f, NumLoopsToPlay, PlayMode);
	}
}

void UUI_Base::StopAnim(const FString& InWidgetAnimName, bool bResetStartPos /*= true*/)
{
	if(UWidgetAnimation* MyTempAnimation = GetNameWidgetAnimation(InWidgetAnimName))
	{
		Super::StopAnimation(MyTempAnimation);
		//if(bResetStartPos)
		//	Super::SetAnimationCurrentTime(MyTempAnimation, 0.0f);
	}
}
