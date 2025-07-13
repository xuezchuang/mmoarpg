// Copyright (C) RenZhai.2021.All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "SimpleRuntimeGamePrintingType.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FSimplePrintSlot
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimplePrintSlot")
	FString Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimplePrintSlot")
	FString Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimplePrintSlot")
	FString ImgID;
};

namespace SimpleRuntimeGamePrinting
{
	struct SIMPLERUNTIMEGAMEPRINTING_API FPrintSlot
	{
		struct SIMPLERUNTIMEGAMEPRINTING_API FSlot
		{
			FSlot& Color(const FString& InColor);
			FSlot& Image(const FString& InImage);

			FSlot& operator[](const FString& InContent)
			{
				if (!ColorContent.IsEmpty())
				{
					Content += FString::Printf(TEXT("<%s>%s</>"), *ColorContent, *InContent);
				}

				return *this;
			}

			const FString& ToString() const;
			bool GetString(FString& OutString) const;

		protected:
			FString Content;
			FString ColorContent;
		};

		FSlot& AddSlot();

		const FString ToString() const;
		bool GetString(FString& OutString) const;

		~FPrintSlot();

	protected:
		TArray<FSlot> Slots;
	};
}

//{
//	FPrintSlot Slot;
//	Slot.AddSlot()
//	.Color("Blue")
//	[
//		"Hello kasd akasd iojasaskod "
//	]
//	.Image("HelloCCCC")
//}