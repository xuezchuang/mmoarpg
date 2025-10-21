// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"
#include "ElementInfo.generated.h"

class ABaseElement;

//Ԫ�ص���Ϣ
USTRUCT()
struct FElementInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = ElementData)
	FText Name;
	UPROPERTY(EditAnywhere, Category = ElementData)
	class UTexture2D* Icon;//Ԫ��ͼ��
	UPROPERTY(EditAnywhere, Category = ElementData)
	FLinearColor Color;//Ԫ�ص���ɫ
	UPROPERTY(EditAnywhere, Category = ElementData)
	TArray<TSubclassOf<ABaseElement>> Enhance;//��ǿЧ����Ԫ��
	UPROPERTY(EditAnywhere, Category = ElementData)
	TArray<TSubclassOf<ABaseElement>> Weakness;//����Ч����Ԫ��
};