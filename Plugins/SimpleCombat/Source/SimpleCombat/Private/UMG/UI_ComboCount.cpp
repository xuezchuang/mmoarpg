//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "UMG/UI_ComboCount.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "UMG/Public/Animation/WidgetAnimation.h"
#include "UMG/Public/Blueprint/WidgetBlueprintGeneratedClass.h"

#define LOCTEXT_NAMESPACE "UUI_ComboCount"

UUI_ComboCount::UUI_ComboCount(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, TimeDelta(0.f)
	, MaxDeltaTime(0.f)
{

}

void UUI_ComboCount::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_ComboCount::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_ComboCount::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry,InDeltaTime);

	if (TimeDelta > 0.f)
	{
		TimeDelta -= InDeltaTime;

		ComboAttackBar->SetPercent(TimeDelta / MaxDeltaTime);

		if (TimeDelta <= 0.f)
		{
			TimeDelta = 0.f;

			PlayWidgetAnim("PlayBar");

			//执行自动销毁
			GetWorld()->GetTimerManager().SetTimer(DieDelayTimeHandle, this, &UUI_ComboCount::Die, 2.f);
		}
	}
}

void UUI_ComboCount::Die()
{
	if (GetWorld()->GetTimerManager().TimerExists(DieDelayTimeHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(DieDelayTimeHandle);
	}

	//移除掉
	RemoveFromParent();
}

void UUI_ComboCount::SetComboText(const int32 Num)
{
	ComboText->SetText(FText::Format(LOCTEXT("ComboText_Key","+{0}"), Num));
}

void UUI_ComboCount::PlayAnim()
{
	PlayWidgetAnim("PlayText");
}

void UUI_ComboCount::SetMaxDeltaTime(float InNewMaxDeltaTime)
{
	MaxDeltaTime = InNewMaxDeltaTime;
	TimeDelta = InNewMaxDeltaTime;

	//重置显示
	ComboAttackBar->SetRenderOpacity(1.f);
}

UWidgetAnimation* UUI_ComboCount::GetNameWidgetAnimation(const FString& InWidgetAnimName)
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

void UUI_ComboCount::PlayWidgetAnim(const FString& InWidgetAnimName)
{
	if (UWidgetAnimation* MyTempAnimation = GetNameWidgetAnimation(InWidgetAnimName))
	{
		PlayAnimation(MyTempAnimation);
	}
}

#undef LOCTEXT_NAMESPACE