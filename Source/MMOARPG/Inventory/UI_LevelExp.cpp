#include "UI_LevelExp.h"

#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Core/Common/MMOARPGGameInstance.h"
#include "Core/Common/MMOARPGNetSubsystem.h"
#include "Protocol/HallProtocol.h"
#include "Protocol/GameProtocol.h"

namespace
{
	void LogLevelExpTextWidgets(UUserWidget* InWidget, const TCHAR* InPrefix)
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

	bool IsTextEquals(const UTextBlock* InTextBlock, const TCHAR* InValue)
	{
		return InTextBlock && InTextBlock->GetText().ToString().TrimStartAndEnd().Equals(InValue);
	}
}

void UUI_LevelExp::NativeConstruct()
{
	Super::NativeConstruct();

	ResolveFallbackBindings();

	UE_LOG(MMOARPG, Display, TEXT("[LevelExpUI] NativeConstruct [Widget:%s] [Level:%p CurrentExp:%p RequiredExp:%p ProgressBar:%p]"),
		*GetName(), Level, CurrentExp, RequiredExp, ProgressBar);
	if (!Level || !CurrentExp || !RequiredExp || !ProgressBar)
	{
		LogLevelExpTextWidgets(this, TEXT("LevelExpUI"));
	}

	if (UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>())
	{
		if (UMMOARPGNetSubsystem* Net = GI->GetSubsystem<UMMOARPGNetSubsystem>())
		{
			InterestingProtos = { SP_CharacterResponse, SP_RoleExp, SP_RoleLevel };
			Net->AddProtoListenerBatch(InterestingProtos, this, &UUI_LevelExp::RecvProtocol, InterestingHandles);
		}
	}

	RefreshDisplay();
}

void UUI_LevelExp::NativeDestruct()
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

void UUI_LevelExp::ResolveFallbackBindings()
{
	if ((Level && CurrentExp && RequiredExp) || !WidgetTree)
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

	int32 LevelLabelIndex = INDEX_NONE;
	int32 ExpLabelIndex = INDEX_NONE;
	int32 SlashIndex = INDEX_NONE;
	for (int32 Index = 0; Index < TextBlocks.Num(); ++Index)
	{
		if (LevelLabelIndex == INDEX_NONE && IsTextEquals(TextBlocks[Index], TEXT("Lv.")))
		{
			LevelLabelIndex = Index;
			continue;
		}

		if (ExpLabelIndex == INDEX_NONE && IsTextEquals(TextBlocks[Index], TEXT("Exp:")))
		{
			ExpLabelIndex = Index;
			continue;
		}

		if (SlashIndex == INDEX_NONE && IsTextEquals(TextBlocks[Index], TEXT("/")))
		{
			SlashIndex = Index;
		}
	}

	if (!Level && TextBlocks.IsValidIndex(LevelLabelIndex + 1))
	{
		Level = TextBlocks[LevelLabelIndex + 1];
	}

	if (!CurrentExp && ExpLabelIndex != INDEX_NONE)
	{
		for (int32 Index = ExpLabelIndex + 1; Index < TextBlocks.Num(); ++Index)
		{
			if (TextBlocks[Index] && !IsTextEquals(TextBlocks[Index], TEXT("/")))
			{
				CurrentExp = TextBlocks[Index];
				break;
			}
		}
	}

	if (!RequiredExp && TextBlocks.IsValidIndex(SlashIndex + 1))
	{
		RequiredExp = TextBlocks[SlashIndex + 1];
	}

	if (!ProgressBar)
	{
		for (UWidget* Widget : AllWidgets)
		{
			if (UProgressBar* CandidateProgressBar = Cast<UProgressBar>(Widget))
			{
				ProgressBar = CandidateProgressBar;
				break;
			}
		}
	}

	ExtraLevelTextBlocks.Reset();
	for (UTextBlock* TextBlock : TextBlocks)
	{
		if (TextBlock && TextBlock != Level && TextBlock != CurrentExp && TextBlock != RequiredExp
			&& !IsTextEquals(TextBlock, TEXT("Lv.")) && !IsTextEquals(TextBlock, TEXT("Exp:")) && !IsTextEquals(TextBlock, TEXT("/")))
		{
			ExtraLevelTextBlocks.Add(TextBlock);
		}
	}

	UE_LOG(MMOARPG, Display, TEXT("[LevelExpUI] ResolveFallbackBindings [Level:%s CurrentExp:%s RequiredExp:%s ProgressBar:%s ExtraLevels:%d]"),
		Level ? *Level->GetName() : TEXT("None"),
		CurrentExp ? *CurrentExp->GetName() : TEXT("None"),
		RequiredExp ? *RequiredExp->GetName() : TEXT("None"),
		ProgressBar ? *ProgressBar->GetName() : TEXT("None"),
		ExtraLevelTextBlocks.Num());
}

void UUI_LevelExp::RefreshDisplay()
{
	ResolveFallbackBindings();

	UMMOARPGGameInstance* GI = GetGameInstance<UMMOARPGGameInstance>();
	if (!GI)
	{
		UE_LOG(MMOARPG, Warning, TEXT("[LevelExpUI] RefreshDisplay skipped because GameInstance is null [Widget:%s]"), *GetName());
		return;
	}

	const int32 CurrentLevel = static_cast<int32>(GI->GetUserData().base.exp.level);
	const int32 CurrentExperience = static_cast<int32>(GI->GetUserData().base.exp.curexp);
	const int32 RequiredExperience = ResolveRequiredExp(CurrentLevel, CurrentExperience);
	const float ProgressPercent = RequiredExperience > 0
		? FMath::Clamp(static_cast<float>(CurrentExperience) / static_cast<float>(RequiredExperience), 0.f, 1.f)
		: 0.f;

	UE_LOG(MMOARPG, Display, TEXT("[LevelExpUI] RefreshDisplay [Widget:%s] [Level:%d Exp:%d/%d Percent:%.3f] [LevelText:%p CurrentExpText:%p RequiredExpText:%p ProgressBar:%p]"),
		*GetName(),
		CurrentLevel,
		CurrentExperience,
		RequiredExperience,
		ProgressPercent,
		Level,
		CurrentExp,
		RequiredExp,
		ProgressBar);

	if (Level)
	{
		Level->SetText(FText::AsNumber(CurrentLevel));
	}

	if (CurrentExp)
	{
		CurrentExp->SetText(FText::AsNumber(CurrentExperience));
	}

	if (RequiredExp)
	{
		RequiredExp->SetText(FText::AsNumber(RequiredExperience));
	}

	if (ProgressBar)
	{
		ProgressBar->SetPercent(ProgressPercent);
	}

	for (UTextBlock* ExtraLevelText : ExtraLevelTextBlocks)
	{
		if (ExtraLevelText)
		{
			ExtraLevelText->SetText(FText::AsNumber(CurrentLevel));
		}
	}

	if (Level && CurrentExp && RequiredExp)
	{
		UE_LOG(MMOARPG, Display, TEXT("[LevelExpUI] Applied Text [Level:%s Exp:%s/%s Progress:%.3f Extra:%d]"),
			*Level->GetText().ToString(),
			*CurrentExp->GetText().ToString(),
			*RequiredExp->GetText().ToString(),
			ProgressBar ? ProgressBar->GetPercent() : -1.f,
			ExtraLevelTextBlocks.Num());
	}
}

void UUI_LevelExp::RecvProtocol(uint32 ProtocolNumber)
{
	UE_LOG(MMOARPG, Display, TEXT("[LevelExpUI] RecvProtocol [%u]"), ProtocolNumber);

	switch (ProtocolNumber)
	{
	case SP_CharacterResponse:
	case SP_RoleExp:
	case SP_RoleLevel:
		RefreshDisplay();
		break;
	default:
		break;
	}
}

int32 UUI_LevelExp::ResolveRequiredExp(int32 InLevel, int32 InCurrentExp) const
{
	const int32 FallbackRequiredExp = FMath::Max(DefaultRequiredExp, InCurrentExp);
	return FMath::Max(FallbackRequiredExp, FMath::Max(1, InLevel) * 100);
}
