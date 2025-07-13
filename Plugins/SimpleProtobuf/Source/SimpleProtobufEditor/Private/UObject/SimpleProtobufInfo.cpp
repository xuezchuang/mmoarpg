// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "UObject/SimpleProtobufInfo.h"

USimpleProtobufObject::USimpleProtobufObject()
{
	auto SpawnDirectory = [](const FString &InPaths)
	{
		FString InDirectory = FPaths::ConvertRelativePathToFull(*InPaths);
		if (!IFileManager::Get().DirectoryExists(*InDirectory))
		{
			IFileManager::Get().MakeDirectory(*InDirectory);
		}
	};

	OutCppPath.Path = TEXT("../../Plugins/SimpleProtobuf/Source/SimpleProtobuf/Public/Protobuf");
	SpawnDirectory(OutCppPath.Path);

	ProtoFiles.Path = FPaths::ProjectDir() / TEXT("Proto");
	SpawnDirectory(ProtoFiles.Path);
}