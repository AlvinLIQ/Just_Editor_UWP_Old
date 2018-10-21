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
const std::wstring IdentifierArray[] = {L"Vector", L"Task" ,L"Array",L"Enum",L"int",L"char",L"if", L"else", L"try", L"catch", L"for",L"while",
L"do",L"#include",L"#define", L"#region", L"#endregion", L"_asm",L"wchar_t",L"size_t",L"unsigned",L"return",L"long",L"short",L"void",L"typedef",
L"#ifdef",L"#endif",L"#ifndef",L"#if", L"string",L"using",L"namespace",L"public",L"private",L"protected",L"virtual",
L"static",L"internal",L"extern",L"new", L"this", L"ref", L"object", L"bool", L"selead", L"var", L"auto" };

DuronSmartDetect::DuronSmartDetect()
{
	InitializeComponent();
	thisData = ref new Editor_Data;
}

void DuronSmartDetect::DetectWordFromStrArray(Windows::UI::Text::ITextRange^ thisRange, bool isHighlight)
{
	ItemPanel->Children->Clear();
	std::wstring thisWord = thisRange->Text->Data();
	size_t thisWordLength = thisWord.length();

	if (thisWordLength)
	{
		wordRange = thisRange;

		thisRange->CharacterFormat->ForegroundColor = thisData->Editor_SymbolColor;
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
					thisRange->CharacterFormat->ForegroundColor = i <= 3 ? thisData->Editor_ForegroundBrush->Color : thisData->IdentifierHighlightColor;
			}
			else if (thisWordLength == count)
			{
				StartIndex = count;
				auto thisItem = ref new DuronWordItem;
				((Grid^)thisItem->Content)->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(
					[this, thisItem](Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
				{
					SelectItem(thisItem);
				});
				((Grid^)thisItem->Content)->DoubleTapped += ref new Windows::UI::Xaml::Input::DoubleTappedEventHandler(
					[this, thisItem, thisRange, isHighlight](Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e)
				{
					thisRange->Text += Editor_Tools::SubPStr(thisItem->Identifier, StartIndex);
					auto thisCodeEditor = ((RichEditBox^)((Grid^)this->Parent)->Children->GetAt(0));
					thisRange->MatchSelection();
					thisCodeEditor->Document->Selection->MoveRight(Windows::UI::Text::TextRangeUnit::Character, 1, false);
					if (isHighlight)
					{
						thisRange->CharacterFormat->ForegroundColor = thisItem->isClass ?
							thisData->Editor_ForegroundBrush->Color : thisData->IdentifierHighlightColor;
					}
					this->SelectedItem = nullptr;
					this->Width = 0;
				});
				thisItem->Identifier = ref new String(IdentifierArray[i].c_str());
				thisItem->OverBrush = thisData->Editor_ForegroundBrush;
				thisItem->ItemIndex = ItemPanel->Children->Size;
				thisItem->isClass = i <= 3;
				if (!ItemPanel->Children->Size)
				{
					thisItem->Select();
					SelectedItem = thisItem;
				}
				if (thisData->isSmartDetectEnabled)
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
	if (MainContent->ScrollableHeight)
	{
		IBox<double>^ y = SelectedItem->ItemIndex * SelectedItem->ActualHeight;
		if (y->Value >= MainContent->ActualHeight || y->Value <= MainContent->VerticalOffset)
		{
			IBox<double>^ x = 0.0;
			IBox<float>^ z = MainContent->ZoomFactor;
			MainContent->ChangeView(x, y, z);
		}
	}
}


void Just_Editor::DuronSmartDetect::UserControl_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	if (this->SelectedItem == nullptr)
		return;

	if (e->Key == Windows::System::VirtualKey::Up)
	{
		this->SelectAt(this->SelectedItem->ItemIndex - 1);
	}
	else if (e->Key == Windows::System::VirtualKey::Down)
	{
		this->SelectAt(this->SelectedItem->ItemIndex + 1);
	}
}
