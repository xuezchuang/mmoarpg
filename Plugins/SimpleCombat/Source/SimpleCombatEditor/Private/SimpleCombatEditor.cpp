//Copyright (C) RenZhai.2022.All Rights Reserved.
#include "SimpleCombatEditor.h"
#include "SimpleCombatEditorStyle.h"
#include "SimpleCombatEditorCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "GameplayTagsSettings.h"
#include "Misc/FileHelper.h"


static const FName SimpleCombatEditorTabName("SimpleCombatEditor");

#define LOCTEXT_NAMESPACE "FSimpleCombatEditorModule"

void FSimpleCombatEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSimpleCombatEditorStyle::Initialize();
	FSimpleCombatEditorStyle::ReloadTextures();

	FSimpleCombatEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSimpleCombatEditorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FSimpleCombatEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSimpleCombatEditorModule::RegisterMenus));
}

void FSimpleCombatEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSimpleCombatEditorStyle::Shutdown();

	FSimpleCombatEditorCommands::Unregister();
}

bool bTagsAlignment = true;

//我们的顺序
void SequentialArrangement(const TArray<FString> &InTags,TArray<FString> &OutTags)
{
	const FString TagsOrderPaths = FPaths::ProjectConfigDir() / TEXT("GameplayTagsOrder.ini");
	if (IFileManager::Get().FileExists(*TagsOrderPaths))
	{
		TArray<FString> TmpTags;
		FFileHelper::LoadFileToStringArray(TmpTags, *TagsOrderPaths);

		if (TmpTags.Num() != 0)
		{
			//找到不同标签
			TArray<FString> DiffTags;
			for (auto& Tmp : TmpTags)
			{
				bool bExit = false;
				for (auto& TmpTag : InTags)
				{
					if (Tmp == TmpTag)
					{
						bExit = true;
						break;
					}
				}

				if (!bExit)
				{
					DiffTags.Add(Tmp);
				}
			}

			//是否需要对齐
			if (bTagsAlignment)
			{
				//Tags对齐操作
				for (auto &Tmp : DiffTags)
				{
					TmpTags.Remove(Tmp);
				}
			}
			else
			{
				for (auto& Tmp : DiffTags)
				{
					int32 Index = INDEX_NONE;
					if (TmpTags.Find(Tmp, Index))
					{
						TmpTags[Index] = FString::Printf(TEXT("INDEX_NONE_%i"), Index);
					}
				}
			}

			OutTags = TmpTags;

			//空缺
			TArray<int32> Position;
			for (auto& TmpTag : OutTags)
			{
				if (TmpTag.Contains(TEXT("INDEX_NONE_")))
				{
					int32 Index = INDEX_NONE;
					if (OutTags.Find(TmpTag, Index))
					{
						Position.AddUnique(Index);
					}
				}
			}

			auto Fill = [&](const FString &InTmpTag)->bool
			{
				if (Position.Num())
				{
					OutTags[Position.Pop()] = InTmpTag;
					return true;
				}

				return false;
			};

			//在原有基础上继续扩展
			for (auto &Tmp : InTags)
			{
				if (!OutTags.Contains(Tmp))
				{
					//添加到空位
					if (!Fill(Tmp))
					{
						//添加到后面
						OutTags.AddUnique(Tmp);
					}
				}
			}
		}
		else
		{
			OutTags = InTags;
		}
	}
	else
	{
		OutTags = InTags;
	}

	//直接存储
	FFileHelper::SaveStringArrayToFile(OutTags, *TagsOrderPaths);
}

void FSimpleCombatEditorModule::PluginButtonClicked()
{
	if (const UGameplayTagsSettings* MutableDefault = GetMutableDefault<UGameplayTagsSettings>())
	{
		TArray<FString> TmpTags;
		for (auto& Tmp : MutableDefault->GameplayTagList)
		{
			TmpTags.Add(Tmp.Tag.ToString());
		}

		//重新制定顺序
		TArray<FString> Tags;
		SequentialArrangement(TmpTags, Tags);

		FString CPPName = FApp::GetProjectName() + FString(TEXT("TagList"));
	//	FString TagsProjectFileName = FPaths::GameSourceDir() / FApp::GetProjectName() / CPPName + TEXT(".h");	
		FString TagsProjectFileName = FPaths::EnginePluginsDir() / TEXT("RENZHAI/MMOARPGCommon/Source/MMOARPGCommon/Public") / CPPName + TEXT(".h");
		
		FString APIString = TEXT("");//FString::Printf(TEXT("%s_API"),*FApp::ProjectName()));
		TArray<FString> TagCodes;
		{
			auto SpawnEnumFunctionCode = [&](int32 Index, int32 StartPos)->int32
			{
				TMap<FName, uint64> TagsMap;
				int32 TagsPos = StartPos;
				FString EnumName = FString::Printf(TEXT("EGamePlayTags%i"), Index);
				TagCodes.Add(TEXT("/*Enumerations can be generated dynamically from gameplaytag*/"));
				TagCodes.Add(FString::Printf(TEXT("enum class %s :uint64"), *EnumName));
				TagCodes.Add(TEXT("{"));
				for (int32 i = StartPos; i < Tags.Num(); i++)
				{
					if (i < (StartPos + 64))
					{
						//Player.Attack.ComboLinkage
						FString InTags = Tags[i];
						InTags.ReplaceInline(TEXT("."), TEXT("_"));

						uint64 InNewValue = 1llu << (i - StartPos);
						TagCodes.Add(FString::Printf(TEXT("	%s	= %llu,"), *InTags, InNewValue));
						TagsPos++;

						TagsMap.Add(*Tags[i], InNewValue);
					}
					else
					{
						break;
					}
				}
				TagCodes.Add(TEXT("};"));
				TagCodes.Add(TEXT(""));

				TagCodes.Add(TEXT("/*Convert gamploytag tag to enumeration*/"));
				TagCodes.Add(FString::Printf(TEXT("%s %s NameTo%s(const FName &InName)"), *EnumName, *APIString, *EnumName));
				TagCodes.Add(TEXT("{"));
				for (auto& Tmp : TagsMap)
				{
					FString EnumValue = Tmp.Key.ToString();
					EnumValue.ReplaceInline(TEXT("."), TEXT("_"));

					TagCodes.Add(FString::Printf(TEXT("	if(InName == TEXT(\"%s\"))"), *Tmp.Key.ToString()));
					TagCodes.Add(TEXT("	{"));
					TagCodes.Add(FString::Printf(TEXT("		 return %s::%s;"), *EnumName, *EnumValue));
					TagCodes.Add(TEXT("	}else"));
				}
				TagCodes.Last().RemoveFromEnd(TEXT("else"));
				TagCodes.Add(TEXT(""));
				TagCodes.Add(FString::Printf(TEXT("	return (%s)0llu; "), *EnumName));
				TagCodes.Add(TEXT("}"));

				TagCodes.Add(TEXT(""));
				TagCodes.Add(TEXT("/*Convert enumeration to gameplaytag.*/"));
				TagCodes.Add(FString::Printf(TEXT("FString %s %sToName(const %s &InTag)"), *APIString, *EnumName, *EnumName));
				TagCodes.Add(TEXT("{"));
				TagCodes.Add(TEXT("	switch(InTag)"));
				TagCodes.Add(TEXT("	{"));
				for (auto& Tmp : TagsMap)
				{
					FString EnumValue = Tmp.Key.ToString();
					EnumValue.ReplaceInline(TEXT("."), TEXT("_"));

					TagCodes.Add(FString::Printf(TEXT("		case %s::%s:"), *EnumName, *EnumValue));
					TagCodes.Add(FString::Printf(TEXT("		 return TEXT(\"%s\");"), *Tmp.Key.ToString()));
					TagCodes.Add(TEXT(""));
				}
				TagCodes.Last().RemoveFromEnd(TEXT(""));
				TagCodes.Add(TEXT("	}"));
				TagCodes.Add(TEXT(""));
				TagCodes.Add(TEXT("	return FString();"));
				TagCodes.Add(TEXT("}"));
				TagCodes.Add(TEXT(""));
				TagCodes.Add(TEXT("/*Convert a composite enumeration to the corresponding label.*/"));
				TagCodes.Add(FString::Printf(TEXT("void %s NameTo%ss(const FName &InName,TArray<FName> &OutName)"), *APIString, *EnumName, *EnumName));
				TagCodes.Add(TEXT("{"));
				TagCodes.Add(TEXT("	uint64 Tags = FCString::Strtoui64(*InName.ToString(),NULL,10);"));
				for (auto& Tmp : TagsMap)
				{
					FString EnumValue = Tmp.Key.ToString();
					EnumValue.ReplaceInline(TEXT("."), TEXT("_"));

					TagCodes.Add(FString::Printf(TEXT("	if(Tags & (int32)%s::%s)"), *EnumName, *EnumValue));
					TagCodes.Add(TEXT("	{"));
					TagCodes.Add(FString::Printf(TEXT("		OutName.AddUnique(TEXT(\"%s\"));"), *Tmp.Key.ToString()));
					TagCodes.Add(TEXT("	}"));
				}
				TagCodes.Add(TEXT("}"));
				TagCodes.Add(TEXT(""));

				TagCodes.Add(TEXT("/*Convert multiple enumerations into one label for storage.*/"));
				TagCodes.Add(FString::Printf(TEXT("FName %s %ssToName(const TArray<FName> &InName)"), *APIString, *EnumName));
				TagCodes.Add(TEXT("{"));
				TagCodes.Add(TEXT("	uint64 Tags = 0llu;"));
				TagCodes.Add(TEXT("	for(auto &Tmp : InName)"));
				TagCodes.Add(TEXT("	{"));
				for (auto& Tmp : TagsMap)
				{
					FString EnumValue = Tmp.Key.ToString();
					EnumValue.ReplaceInline(TEXT("."), TEXT("_"));

					TagCodes.Add(FString::Printf(TEXT("		if(Tmp == FName(TEXT(\"%s\")))"), *Tmp.Key.ToString()));
					TagCodes.Add(TEXT("		{"));
					TagCodes.Add(FString::Printf(TEXT("			Tags |= (uint64)%s::%s;"), *EnumName, *EnumValue));
					TagCodes.Add(TEXT("		}else"));
				}
				TagCodes.Last().RemoveFromEnd(TEXT("else"));

				TagCodes.Add(TEXT("	}"));
				TagCodes.Add(TEXT(""));
				TagCodes.Add(TEXT("	return *FString::Printf(TEXT(\"%llu\"),Tags);"));
				TagCodes.Add(TEXT("}"));

				return TagsPos;
			};

			float InValue = (float)Tags.Num() / 64.f;
			int32 InTotal = FMath::CeilToInt(InValue);
			int32 TagsPos = 0;

			TagCodes.Add(TEXT("//Copyright (C) RenZhai.2022.All Rights Reserved."));
			TagCodes.Add(TEXT(""));
			TagCodes.Add(TEXT("#pragma once"));
			TagCodes.Add(TEXT("#include \"CoreMinimal.h\""));
			TagCodes.Add(TEXT(""));
			TagCodes.Add(TEXT("//The file used to reflect gameplay tags is used."));
			TagCodes.Add(TEXT(""));

			TagCodes.Add(TEXT(""));
			TagCodes.Add(TEXT("/*A static function that contains a list of types order*/"));
			TagCodes.Add(TEXT("int32 GetGamePlayTagOrderIndex(const FName &InTag)"));
			TagCodes.Add(TEXT("{"));
			TagCodes.Add(TEXT("	static TArray<FString> GameplayTagsOrder = "));
			TagCodes.Add(TEXT("	{"));
			for (auto& Tmp : Tags)
			{
				TagCodes.Add(FString::Printf(TEXT("		\"%s\","), *Tmp));
			}
			TagCodes.Add(TEXT("	};"));
			TagCodes.Add(TEXT(""));
			TagCodes.Add(TEXT("	return GameplayTagsOrder.Find(InTag.ToString());"));
			TagCodes.Add(TEXT("}"));

			TagCodes.Add(TEXT(""));
			TagCodes.Add(TEXT("/*Get the number of enumerations of the current order*/"));
			TagCodes.Add(TEXT("int32 GetOrderMatchingEnumIndex(int32 InGameplayTagsOrderIndex)"));
			TagCodes.Add(TEXT("{"));
			TagCodes.Add(TEXT("	if (InGameplayTagsOrderIndex == 0)"));
			TagCodes.Add(TEXT("	{"));
			TagCodes.Add(TEXT("		return 1;"));
			TagCodes.Add(TEXT("	}"));
			TagCodes.Add(TEXT(""));
			TagCodes.Add(TEXT("	return FMath::CeilToInt((float)InGameplayTagsOrderIndex / 63.f);"));
			TagCodes.Add(TEXT("}"));

			TagCodes.Add(TEXT(""));

			for (int32 i = 0; i < InTotal; i++)
			{
				TagsPos = SpawnEnumFunctionCode(i,TagsPos);
			}

			TagCodes.Add(TEXT(""));
			TagCodes.Add(TEXT("//////////////////////////Main/////////////////////////////"));
			TagCodes.Add(TEXT("/*Convert BitAndOrder to bytes and TagName*/"));
			TagCodes.Add(TEXT("bool SingleBitAndOrderToTagName(const int32 InEnumIndex, const uint64 EnumValue,FName &TagName)"));
			TagCodes.Add(TEXT("{"));
			TagCodes.Add(TEXT("	switch (InEnumIndex)"));
			TagCodes.Add(TEXT("	{"));
			for (int32 i = 0; i < InTotal; i++)
			{
				FString EnumName = FString::Printf(TEXT("EGamePlayTags%i"), i);

				TagCodes.Add(FString::Printf(TEXT("	case %i:"), i));
				TagCodes.Add(FString::Printf(TEXT("		TagName = *%sToName((%s)EnumValue);"), *EnumName, *EnumName));
				TagCodes.Add(TEXT("		return true;"));
			}
			TagCodes.Add(TEXT("	}"));
			TagCodes.Add(TEXT(""));
			TagCodes.Add(TEXT("	return false;"));
			TagCodes.Add(TEXT("}"));
			TagCodes.Add(TEXT(""));

			TagCodes.Add(TEXT("/*Convert FName to bytes and order*/"));
			TagCodes.Add(TEXT("bool SingleTagNameToBitAndOrder(const FName& TageName, int32& InEnumIndex, uint64& EnumValue)"));
			TagCodes.Add(TEXT("{"));
			TagCodes.Add(TEXT("	int32 InOrder = GetGamePlayTagOrderIndex(TageName);"));
			TagCodes.Add(TEXT("	if (InOrder != INDEX_NONE)"));
			TagCodes.Add(TEXT("	{"));
			TagCodes.Add(TEXT("		InEnumIndex = GetOrderMatchingEnumIndex(InOrder) - 1;"));
			TagCodes.Add(TEXT("		switch (InEnumIndex)"));
			TagCodes.Add(TEXT("		{"));
			for (int32 i = 0; i < InTotal; i++)
			{
				FString EnumName = FString::Printf(TEXT("EGamePlayTags%i"), i);

				TagCodes.Add(FString::Printf(TEXT("		case %i:"), i));
				TagCodes.Add(FString::Printf(TEXT("			EnumValue = (uint64)NameTo%s(TageName);"), *EnumName));
				TagCodes.Add(TEXT("			return true;"));
			}
			TagCodes.Add(TEXT("		}"));
			TagCodes.Add(TEXT("	}"));
			TagCodes.Add(TEXT(""));
			TagCodes.Add(TEXT("	return true;"));
			TagCodes.Add(TEXT("}"));

			TagCodes.Add(TEXT(""));
			TagCodes.Add(TEXT("/*Convert the server's sequence to a label.*/"));
			TagCodes.Add(FString::Printf(TEXT("void %s AnalysisArrayNameToGamePlayTags(const TArray<FName> &InNames,TArray<FName> &OutNames)"), *APIString));
			TagCodes.Add(TEXT("{"));
			for (int32 i = 0; i < InTotal; i++)
			{
				FString EnumName = FString::Printf(TEXT("EGamePlayTags%i"), i);

				TagCodes.Add(FString::Printf(TEXT("	for(auto &Tmp : InNames)")));
				TagCodes.Add(TEXT("	{"));
				TagCodes.Add(FString::Printf(TEXT("		NameTo%ss(Tmp,OutNames);"), *EnumName));
				TagCodes.Add(TEXT("	}"));
			}
			TagCodes.Add(TEXT("}"));
			TagCodes.Add(TEXT(""));

			TagCodes.Add(TEXT("/*Convert multiple tags to a sequence stored by the server.*/"));
			TagCodes.Add(FString::Printf(TEXT("void %s AnalysisGamePlayTagsToArrayName(const TArray<FName> &InNames,TArray<FName> &OutNames)"), *APIString));
			TagCodes.Add(TEXT("{"));
			for (int32 i = 0; i < InTotal; i++)
			{
				FString EnumName = FString::Printf(TEXT("EGamePlayTags%i"), i);
				TagCodes.Add(FString::Printf(TEXT("	FName TagName = %ssToName(InNames);"), *EnumName));
				TagCodes.Add(TEXT("	if(TagName != NAME_None)"));
				TagCodes.Add(TEXT("	{"));
				TagCodes.Add(TEXT("		OutNames.AddUnique(TagName);"));
				TagCodes.Add(TEXT("	}"));
			}
			TagCodes.Add(TEXT("}"));
		}

		FFileHelper::SaveStringArrayToFile(TagCodes, *TagsProjectFileName);
	}

	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
								LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
								FText::FromString(TEXT("FSimpleCombatEditorModule::PluginButtonClicked()")),
								FText::FromString(TEXT("SimpleCombatEditor.cpp"))
	);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);	
}

void FSimpleCombatEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FSimpleCombatEditorCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSimpleCombatEditorCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimpleCombatEditorModule, SimpleCombatEditor)