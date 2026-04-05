#pragma once

#include "CoreMinimal.h"
#include "FItemData.h"
#include "Engine/Texture2D.h"
#include "FCharacterMenuData.generated.h"

USTRUCT(BlueprintType)
struct FCharacterMenuTabData
{
	GENERATED_BODY()

public:
	FCharacterMenuTabData() = default;

	FCharacterMenuTabData(E_UIType InTabType, const FText& InDisplayName, UTexture2D* InIcon, int32 InWidgetIndex)
		: TabType(InTabType)
		, DisplayName(InDisplayName)
		, Icon(InIcon)
		, WidgetIndex(InWidgetIndex)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMenu")
	E_UIType TabType = E_UIType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMenu")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMenu")
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMenu", meta = (ClampMin = "0"))
	int32 WidgetIndex = 0;
};

USTRUCT(BlueprintType)
struct FCharacterMenuConfig
{
	GENERATED_BODY()

public:
	FCharacterMenuConfig() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMenu")
	E_UIType DefaultTab = E_UIType::Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMenu")
	TArray<FCharacterMenuTabData> Tabs;
};
