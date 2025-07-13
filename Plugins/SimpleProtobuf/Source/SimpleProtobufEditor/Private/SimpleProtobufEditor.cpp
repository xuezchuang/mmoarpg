// Copyright (C) RenZhai.2021.All Rights Reserved.

#include "SimpleProtobufEditor.h"
#include "SimpleProtobufEditorStyle.h"
#include "SimpleProtobufEditorCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "ISettingsModule.h"
#include "UObject/SimpleProtobufInfo.h"

static const FName SimpleProtobufEditorTabName("SimpleProtobufEditor");

#define LOCTEXT_NAMESPACE "FSimpleProtobufEditorModule"

void FSimpleProtobufEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSimpleProtobufEditorStyle::Initialize();
	FSimpleProtobufEditorStyle::ReloadTextures();

	FSimpleProtobufEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSimpleProtobufEditorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FSimpleProtobufEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSimpleProtobufEditorModule::RegisterMenus));

	//注册到projectsetting
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings")))
	{
		SettingsModule->RegisterSettings("Project","Protobuf","ProtobufSettings",
			LOCTEXT("ProtobufSettings", "Protobuf Settings"),
			LOCTEXT("ProtobufSettingsDescription", "Mainly used to generate c++ protocol files."),
			GetMutableDefault<USimpleProtobufObject>());
	}
}

void FSimpleProtobufEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSimpleProtobufEditorStyle::Shutdown();

	FSimpleProtobufEditorCommands::Unregister();
}

void FSimpleProtobufEditorModule::PluginButtonClicked()
{
	FText ProtoInfo;

	//I 找到所有.proto文件
	FString ProtoFiles = FPaths::ConvertRelativePathToFull(GetMutableDefault<USimpleProtobufObject>()->ProtoFiles.Path);
	FString OutCppPath = FPaths::ConvertRelativePathToFull(GetMutableDefault<USimpleProtobufObject>()->OutCppPath.Path);

	TArray<FString> FileNames;
	IFileManager::Get().FindFilesRecursive(FileNames, *ProtoFiles, TEXT("*.*"), true, false);

	FString ProtoBufcExe = FPaths::ConvertRelativePathToFull(TEXT("../../Plugins/RENZHAI/SimpleProtobuf/Source/SimpleProtobuf/Public/exe"));
	if (!IFileManager::Get().DirectoryExists(*ProtoBufcExe))
	{
		ProtoBufcExe = FPaths::ConvertRelativePathToFull(TEXT("../../Plugins/SimpleProtobuf/Source/SimpleProtobuf/Public/exe"));
		if (!IFileManager::Get().DirectoryExists(*ProtoBufcExe))
		{
			ProtoInfo = FText::FromString(ProtoInfo.ToString() +
				FString::Printf(TEXT("The executor path does not exist.[%s]"),*ProtoBufcExe));
			return ;
		}
	}

	//II 遍历.proto文件
	for (auto& Tmp : FileNames)
	{
		//Tmp = C:\Users\Administrator\Pictures\new\SimpleProtobuf\dd.proto
		FString HeadFile = FPaths::GetCleanFilename(Tmp);
		if (HeadFile.Contains(TEXT(".proto")))
		{
			//../../Plugins/SimpleProtobuf/Source/SimpleProtobuf/Public/bin/dd.proto
			FString ProtoBufcProto = ProtoBufcExe / HeadFile;

			//III
			IFileManager::Get().Copy(*ProtoBufcProto, *Tmp);
			FString Parma = TEXT("--cpp_out=") + OutCppPath + TEXT(" ./") + HeadFile;

			int32 ReturnCode;
			FString OutStdOut;
			FString ErrorMsg;

			//IV 执行命令
			if (FPlatformProcess::ExecProcess(*(ProtoBufcExe / TEXT("protoc.exe")), *Parma,
				&ReturnCode, &OutStdOut, &ErrorMsg, *ProtoBufcExe))
			{
				
			}

			if (ReturnCode == 0)
			{
				ProtoInfo = FText::FromString(ProtoInfo.ToString() + 
					FString::Printf(TEXT("Std:%s . \n Err:%s .\n Success!"), *OutStdOut, *ErrorMsg));
			}
			else
			{
				ProtoInfo = FText::FromString(ProtoInfo.ToString() +
					FString::Printf(TEXT("Std:%s . \n Err:%s .\n fail!"), *OutStdOut, *ErrorMsg));
			}

			IFileManager::Get().Delete(*ProtoBufcProto);
		}
	}
	
	if (ProtoInfo.IsEmpty())
	{
		ProtoInfo = FText::FromString(TEXT("Specify at least one file with .proto."));
	}

	//V 打印
	FMessageDialog::Open(EAppMsgType::Ok, ProtoInfo);
}

void FSimpleProtobufEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FSimpleProtobufEditorCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSimpleProtobufEditorCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimpleProtobufEditorModule, SimpleProtobufEditor)