// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SelectableInterface.generated.h"

UINTERFACE()
class USelectableInterface :public UInterface {
	GENERATED_BODY()
};

class ISelectableInterface {
	GENERATED_BODY()

public:
	virtual void OnSelected(class ARBaseCharacter* Character) = 0;//选中的时候
	virtual void OnSelectionEnd(class ARBaseCharacter* Character) = 0;//选择之后离开
};