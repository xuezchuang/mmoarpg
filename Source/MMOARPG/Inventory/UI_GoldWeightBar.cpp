#include "UI_GoldWeightBar.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Core/Common/MMOARPGGameInstance.h"
#include "Core/Common/MMOARPGNetSubsystem.h"
#include "Core/Game/MMOARPGPlayerState.h"
#include "Data/FItemData.h"
#include "Protocol/HallProtocol.h"
#include "Protocol/GameProtocol.h"

namespace
{
	void LogTextWidgets(UUserWidget* InWidget, const TCHAR* InPrefix)
	{
		if (!InWidget || !InWidget->WidgetTree)
		{
			return;
		}

		TArray<UWidget*> AllWidgets;
		InWidget->WidgetTree->GetAllWidgets(AllWidgets);
		for (UWidget* Widget : AllWidgets)
		{
			if (UTextBlock* TextWidget = Cast<UTextBlock>(Widget))
			{
				UE_LOG(MMOARPG, Warning, TEXT("[%s] TextWidget [%s] Text=[%s]"), InPrefix, *TextWidget->GetName(), *TextWidget->GetText().ToString());
			}
		}
	}

	bool IsSlashText(const UTextBlock* InTextBlock)
	{
		return InTextBlock && InTextBlock->GetText().ToString().TrimStartAndEnd().Equals(TEXT("/"));
	}
}

void UUI_GoldWeightBar::NativeConstruct()
{
	Super::NativeConstruct();

	ResolveFallbackBindings();

	UE_LOG(MMOARPG, Display, TEXT("[GoldWeightUI] NativeConstruct [Widget:%s] [Gold:%p CurrentWeight:%p MaxWeight:%p]"),
		*GetName(), Gold, CurrentWeight, MaxWeight);
	if (!Gold || !CurrentWeight || !MaxWeight)
	{
		LogTextWidgets(this, TEXT("GoldWeightUI"));
	}

	if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
	{
		if (UMMOARPGNetSubsystem* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
		{
			InterestingProtos = { SP_CharacterResponse, SP_RoleCurrency };
			Net->AddProtoListenerBatch(InterestingProtos, this, &UUI_GoldWeightBar::RecvProtocol, InterestingHandles);
		}
	}

	RefreshDisplay();
}

void UUI_GoldWeightBar::NativeDestruct()
{
	if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
	{
		if (UMMOARPGNetSubsystem* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
		{
			Net->RemoveProtoListenersBatch(InterestingProtos, InterestingHandles);
		}
	}

	InterestingProtos.Reset();
	InterestingHandles.Reset();

	Super::NativeDestruct();
}

void UUI_GoldWeightBar::ResolveFallbackBindings()
{
	if ((Gold && CurrentWeight && MaxWeight) || !WidgetTree)
	{
		return;
	}

	TArray<UWidget*> AllWidgets;
	WidgetTree->GetAllWidgets(AllWidgets);

	TArray<UTextBlock*> TextBlocks;
	for (UWidget* Widget : AllWidgets)
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlocks.Add(TextBlock);
		}
	}

	if (TextBlocks.Num() == 0)
	{
		return;
	}

	int32 SlashIndex = INDEX_NONE;
	for (int32 Index = 0; Index < TextBlocks.Num(); ++Index)
	{
		if (IsSlashText(TextBlocks[Index]))
		{
			SlashIndex = Index;
			break;
		}
	}

	if (!CurrentWeight && SlashIndex > 0)
	{
		CurrentWeight = TextBlocks[SlashIndex - 1];
	}

	if (!MaxWeight && SlashIndex != INDEX_NONE && TextBlocks.IsValidIndex(SlashIndex + 1))
	{
		MaxWeight = TextBlocks[SlashIndex + 1];
	}

	if (!Gold)
	{
		for (int32 Index = 0; Index < TextBlocks.Num(); ++Index)
		{
			UTextBlock* Candidate = TextBlocks[Index];
			if (Candidate && Candidate != CurrentWeight && Candidate != MaxWeight && !IsSlashText(Candidate))
			{
				Gold = Candidate;
				break;
			}
		}
	}

	UE_LOG(MMOARPG, Display, TEXT("[GoldWeightUI] ResolveFallbackBindings [Gold:%s CurrentWeight:%s MaxWeight:%s]"),
		Gold ? *Gold->GetName() : TEXT("None"),
		CurrentWeight ? *CurrentWeight->GetName() : TEXT("None"),
		MaxWeight ? *MaxWeight->GetName() : TEXT("None"));
}

void UUI_GoldWeightBar::RefreshDisplay()
{
	ResolveFallbackBindings();

	UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>();
	const int32 GoldValue = GI ? static_cast<int32>(GI->GetUserData().base.econ.gold) : 0;
	const int32 DisplayGoldValue = FMath::Min(GoldValue, FMath::Max(0, MaxDisplayGold));
	const float CurrentWeightValue = ResolveCurrentWeight();
	const float MaxWeightValue = ResolveMaxWeight();

	UE_LOG(MMOARPG, Display, TEXT("[GoldWeightUI] RefreshDisplay [Widget:%s] [HasGI:%d HasPS:%d] [Gold:%d DisplayGold:%d Weight:%.2f/%.2f] [GoldText:%p CurrentWeightText:%p MaxWeightText:%p]"),
		*GetName(),
		GI != nullptr ? 1 : 0,
		GetPlayerState<AMMOARPGPlayerState>() != nullptr ? 1 : 0,
		GoldValue,
		DisplayGoldValue,
		CurrentWeightValue,
		MaxWeightValue,
		Gold,
		CurrentWeight,
		MaxWeight);

	if (Gold && GI)
	{
		Gold->SetText(FText::AsNumber(DisplayGoldValue));
	}

	if (CurrentWeight)
	{
		CurrentWeight->SetText(FormatWeightText(CurrentWeightValue));
	}

	if (MaxWeight)
	{
		MaxWeight->SetText(FormatWeightText(MaxWeightValue));
	}

	if (Gold)
	{
		UE_LOG(MMOARPG, Display, TEXT("[GoldWeightUI] Applied Text [Gold:%s]"), *Gold->GetText().ToString());
	}

	if (CurrentWeight && MaxWeight)
	{
		UE_LOG(MMOARPG, Display, TEXT("[GoldWeightUI] Applied Weight Text [%s/%s]"),
			*CurrentWeight->GetText().ToString(),
			*MaxWeight->GetText().ToString());
	}
}

void UUI_GoldWeightBar::RecvProtocol(uint32 ProtocolNumber)
{
	UE_LOG(MMOARPG, Display, TEXT("[GoldWeightUI] RecvProtocol [%u]"), ProtocolNumber);

	switch (ProtocolNumber)
	{
	case SP_CharacterResponse:
	case SP_RoleCurrency:
		RefreshDisplay();
		break;
	default:
		break;
	}
}

float UUI_GoldWeightBar::ResolveCurrentWeight()
{
	AMMOARPGPlayerState* PlayerState = GetPlayerState<AMMOARPGPlayerState>();
	if (!PlayerState)
	{
		return 0.f;
	}

	double TotalWeight = 0.0;
	for (const FFS_ItemData& Item : PlayerState->GetBagItems())
	{
		const int32 Quantity = FMath::Max(1, Item.Stacks.Quantity);
		TotalWeight += static_cast<double>(Item.Stats.Weight) * static_cast<double>(Quantity);
	}

	return static_cast<float>(TotalWeight);
}

float UUI_GoldWeightBar::ResolveMaxWeight() const
{
	return FMath::Max(0.f, DefaultMaxWeight);
}

FText UUI_GoldWeightBar::FormatWeightText(float InValue)
{
	FNumberFormattingOptions FormatOptions;
	FormatOptions.MinimumFractionalDigits = 0;
	FormatOptions.MaximumFractionalDigits = FMath::IsNearlyEqual(InValue, FMath::RoundToFloat(InValue), KINDA_SMALL_NUMBER) ? 0 : 2;
	return FText::AsNumber(InValue, &FormatOptions);
}
