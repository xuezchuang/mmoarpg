#include "DetailsMapping/SimpleSelectStringDetail.h"
#include "DetailWidgetRow.h"
#include "SimpleNumericalDeductionType.h"

#define LOCTEXT_NAMESPACE "FSimpleSelectStringDetail"

void FSimpleSelectStringDetail::HandleCharacterKey(const FString InKey)
{
	MySelectText = FText::FromString(InKey);

	if (SelectKey.IsValid())
	{
		SelectKey->SetValueFromFormattedString(InKey);
	}
}

void FSimpleSelectStringDetail::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	FMenuBuilder CharacterAttributeModeBuilder(true, NULL);

	//ÄÃµ½Key
	SelectKey = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSimpleSelectString, SelectString));
	
	if (TSharedPtr<IPropertyHandle> StringsArrayPropertyHandle = PropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FSimpleSelectString, SelectStrings)))
	{
		if (TSharedPtr<IPropertyHandleArray> ArrayHandle = StringsArrayPropertyHandle->AsArray())
		{
			TArray<FString> MyStrings;

			uint32 Number = 0;
			ArrayHandle->GetNumElements(Number);
			for (uint32 i = 0; i < Number; i++)
			{
				if (TSharedPtr<IPropertyHandle> StringElementHandle = ArrayHandle->GetElement(i))
				{
					FString &MyString = MyStrings.AddDefaulted_GetRef();
					StringElementHandle->GetValueAsFormattedString(MyString);
				}
			}

			for (size_t i = 0; i < MyStrings.Num(); i++)
			{
				FUIAction CharacterAction(FExecuteAction::CreateSP(
					this,
					&FSimpleSelectStringDetail::HandleCharacterKey, MyStrings[i]));

				CharacterAttributeModeBuilder.AddMenuEntry(
					FText::FromString(MyStrings[i]),
					LOCTEXT("HelloTip", "HelloTip"),
					FSlateIcon(), CharacterAction);
			}
		}
	}

	HeaderRow
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(125.0f)
	.MaxDesiredWidth(325.0f)
	[
		SNew(SComboButton)
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(this, &FSimpleSelectStringDetail::SelectText)
		]
		.MenuContent()
		[
			CharacterAttributeModeBuilder.MakeWidget()
		]
	];
}

void FSimpleSelectStringDetail::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{

}

TSharedRef<IPropertyTypeCustomization> FSimpleSelectStringDetail::MakeInstance()
{
	return MakeShareable(new FSimpleSelectStringDetail());
}

FText FSimpleSelectStringDetail::SelectText() const
{
	return MySelectText;
}

#undef LOCTEXT_NAMESPACE