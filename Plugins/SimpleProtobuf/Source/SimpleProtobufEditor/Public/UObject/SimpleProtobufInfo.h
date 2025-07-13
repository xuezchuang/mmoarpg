// Copyright (C) RenZhai.2021.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SimpleProtobufInfo.generated.h"

UCLASS(config = USimpleProtobufObject)
class USimpleProtobufObject :public UObject
{
	GENERATED_BODY()

public:
	USimpleProtobufObject();

	UPROPERTY(config, EditAnywhere, Category = "Protobuf", meta = (ToolTip = "Point to protobuf file location."))
	FDirectoryPath ProtoFiles;

	UPROPERTY(config, EditAnywhere, Category = "Protobuf", meta = (ToolTip = "Output C++ file location."))
	FDirectoryPath OutCppPath;
};