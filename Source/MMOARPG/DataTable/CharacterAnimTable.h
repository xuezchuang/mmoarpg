
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterAnimTable.generated.h"

USTRUCT(BlueprintType)
struct FCharacterAnimTable :public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FCharacterAnimTable();

	UPROPERTY(EditDefaultsOnly, Category = "CharacterAnim")
	int32 ID;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterAnim")
	UAnimMontage* SwitchFightMontage;
};