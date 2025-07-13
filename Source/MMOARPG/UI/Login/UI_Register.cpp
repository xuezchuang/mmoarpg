// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_Register.h"
#include "UI_LoginMain.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "UUI_Register"

void UUI_Register::NativeConstruct()
{
	Super::NativeConstruct();

	SubmitButton->OnReleased.AddDynamic(this, &UUI_Register::Submit);
	CancelButton->OnReleased.AddDynamic(this, &UUI_Register::Cancel);

	ResetColor();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UUI_Register::NativeDestruct()
{
	Super::NativeDestruct();

	
}

void UUI_Register::Submit()
{
	if (UUI_LoginMain* InLoginMain = GetParents<UUI_LoginMain>())
	{
		ResetColor();

		FString NiceNameString = NiceName->GetText().ToString();
		FString EmailString = Email->GetText().ToString();
		FString PlayerAccountString = PlayerAccount->GetText().ToString();
		FString PlayerPasswordString = PlayerPassword->GetText().ToString();
		FString ConfirmPasswordString = ConfirmPassword->GetText().ToString();
		FString AuthorURLString = AuthorURL->GetText().ToString();

		bool bFailedtoRegisterWarnings = true;
		if (NiceNameString.Len() < 3)
		{
			ShowNiceNameWarnings();
			
			InLoginMain->PrintLog(LOCTEXT("Register_NiceName", "The name has at least 3 characters."));
		
		}
		//else if (PlayerAccountString.Len() < 6)
		//{
		//	ShowAccountWarnings();
		//	
		//	InLoginMain->PrintLog(LOCTEXT("Register_Account", "The account has at least 6 characters."));
		//}
		//else if (!EmailString.Contains(TEXT(".")) || !EmailString.Contains(TEXT("@")))
		//{
		//	ShowEmailWarnings();

		//	InLoginMain->PrintLog(LOCTEXT("Register_Email", "The mailbox format is incorrect."));
		//}
		else if (PlayerPasswordString.Len() < 6)
		{
			ShowPasswordWarnings();
			
			InLoginMain->PrintLog(LOCTEXT("Register_Password", "The password has at least 6 characters."));
		}
		//else if (PlayerPasswordString != ConfirmPasswordString)
		//{
		//	ShowConfirmPasswordWarnings();
		//	
		//	InLoginMain->PrintLog(LOCTEXT("Register_ConfirmPassword", "The password is inconsistent."));
		//}
		else
		{
			//FString RegisterInfo = 
			//	FString::Printf(TEXT(
			//		"NiceName=%s&Email=%s&Account=%s&Password=%s&AuthorURL=%s"),
			//	*NiceNameString,
			//	*EmailString,
			//	*PlayerAccountString,
			//	*PlayerPasswordString,
			//	*AuthorURLString);

			InLoginMain->Register(NiceNameString, PlayerPasswordString);

			RegisterOut();

			bFailedtoRegisterWarnings = false;
		}

		if (bFailedtoRegisterWarnings)
		{
			ShowFailedtoRegisterWarnings();
		}
	}
}

void UUI_Register::Cancel()
{
	RegisterOut();
}

void UUI_Register::ResetText()
{
	NiceName->SetText(FText::GetEmpty());
	PlayerAccount->SetText(FText::GetEmpty());
	PlayerPassword->SetText(FText::GetEmpty());
	ConfirmPassword->SetText(FText::GetEmpty());
	AuthorURL->SetText(FText::GetEmpty());
	Email->SetText(FText::GetEmpty());
}

void UUI_Register::ResetColor()
{
	NiceNameText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	PlayerAccountText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	PlayerPasswordText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ConfirmPasswordText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	EmailText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
}

void UUI_Register::RegisterIn()
{
	PlayWidgetAnim(TEXT("RegisterIn"));
}

void UUI_Register::RegisterOut()
{
	PlayWidgetAnim(TEXT("RegisterOut"));
}

void UUI_Register::ShowFailedtoRegisterWarnings()
{
	PlayWidgetAnim(TEXT("FailedtoRegisterAnim"));
}

void UUI_Register::ShowNiceNameWarnings()
{
	NiceNameText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	PlayWidgetAnim(TEXT("NiceNameAnim"));
}

void UUI_Register::ShowAccountWarnings()
{
	PlayerAccountText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	PlayWidgetAnim(TEXT("AccountAnim"));
}

void UUI_Register::ShowEmailWarnings()
{
	EmailText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	PlayWidgetAnim(TEXT("EmailANim"));
}

void UUI_Register::ShowPasswordWarnings()
{
	PlayerPasswordText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	PlayWidgetAnim(TEXT("PasswordAnim"));
}

void UUI_Register::ShowConfirmPasswordWarnings()
{
	ConfirmPasswordText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	PlayWidgetAnim(TEXT("ConfirmPasswordAnim"));
}

void UUI_Register::ShowDuplicateWarnings()
{
	ShowEmailWarnings();
	ShowAccountWarnings();
}

#undef LOCTEXT_NAMESPACE