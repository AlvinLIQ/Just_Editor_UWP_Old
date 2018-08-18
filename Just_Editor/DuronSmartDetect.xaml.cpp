//
// DuronSmartDetect.xaml.cpp
// Implementation of the DuronSmartDetect class
//

#include "pch.h"
#include "DuronSmartDetect.xaml.h"
#include "Editor_Tools.h"

using namespace Just_Editor;

using namespace concurrency;
using namespace Platform;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236
const std::wstring IdentifierArray[] = { L"[code]", L"[/code]" ,L"[image]",L"[/image]",L"int",L"char",L"if",L"for",L"while",
L"do",L"#include",L"#define",L"_asm",L"wchar_t",L"size_t",L"unsigned",L"return",L"long",L"short",L"void",L"typedef",
L"#ifdef",L"#endif",L"#ifndef",L"#if",L"string",L"using",L"namespace",L"public",L"private",L"protected",L"virtual",
L"static",L"internal",L"extern",L"new", L"this", L"ref", L"object", L"bool", L"selead", L"var", L"auto" };

DuronSmartDetect::DuronSmartDetect()
{
	InitializeComponent();
}

void DuronSmartDetect::DetectWordFromStrArray(Windows::UI::Text::ITextRange^ thisRange, bool isHighlight)
{
	ItemPanel->Children->Clear();
	std::wstring thisWord = thisRange->Text->Data();
	size_t thisWordLength = thisWord.length();

	if (thisWordLength)
	{
		wordRange = thisRange;
		if (isHighlight)
			thisRange->CharacterFormat->ForegroundColor = Windows::UI::Colors::Gray;
		int count = 0;

		for (size_t i = 0, j, thisIDLength; i < IdentifierNum; i++, count = 0)
		{
			thisIDLength = IdentifierArray[i].length();
			if (thisIDLength < thisWordLength)
			{
				continue;
			}
			for (j = 0; j < thisWordLength; j++)
			{
				if (thisWord[j] == IdentifierArray[i][j])
					count++;
				else
					break;
			}
			if (thisIDLength == count)
			{
				if (isHighlight)
					thisRange->CharacterFormat->ForegroundColor = Windows::UI::Colors::MediumBlue;
			}
			else if (thisWordLength == count)
			{
				StartIndex = count;
				auto thisItem = ref new DuronWordItem;
				((Grid^)thisItem->Content)->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler([this, thisItem](Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
				{
					SelectItem(thisItem);
				});
				((Grid^)thisItem->Content)->DoubleTapped += ref new Windows::UI::Xaml::Input::DoubleTappedEventHandler([this, thisItem, thisRange, isHighlight](Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e)
				{
					thisRange->Text += Editor_Tools::SubPStr(thisItem->Identifier, StartIndex);
					auto thisCodeEditor = ((RichEditBox^)((Grid^)this->Parent)->Children->GetAt(0));
					thisRange->MatchSelection();
					thisCodeEditor->Document->Selection->MoveRight(Windows::UI::Text::TextRangeUnit::Character, 1, false);
					if (isHighlight)
						thisRange->CharacterFormat->ForegroundColor = Windows::UI::Colors::MediumBlue;
					this->SelectedItem = nullptr;
					this->Width = 0;
				});
				thisItem->Identifier = ref new String(IdentifierArray[i].c_str());
				thisItem->OverBrush = MainContent->Background;
				thisItem->ItemIndex = ItemPanel->Children->Size;
				if (!ItemPanel->Children->Size)
				{
					thisItem->Select();
					SelectedItem = thisItem;
				}

				ItemPanel->Children->Append(thisItem);
			}
		}
	}
	if (ItemPanel->Children->Size)
	{
		this->Width = 100;
	}
	else
	{
		SelectedItem = nullptr;
		this->Width = 0;
	}
}

void DuronSmartDetect::SelectAt(unsigned int ItemIndex)
{
	if (ItemIndex >= ItemPanel->Children->Size)
		return;

	SelectItem((DuronWordItem^)ItemPanel->Children->GetAt(ItemIndex));
}

void DuronSmartDetect::SelectItem(DuronWordItem^ thisItem)
{
	if (SelectedItem != nullptr)
	{
		SelectedItem->UnSelect();
	}

	thisItem->Select();
	SelectedItem = thisItem;
}
