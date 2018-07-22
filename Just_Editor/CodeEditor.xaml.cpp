//
// CodeEditor.xaml.cpp
// Implementation of the CodeEditor class
//

#include "pch.h"
#include "CodeEditor.xaml.h"
#include "Editor_Tools.h"

const wchar_t* MyIDentifier[] = { L"[code]", L"[/code]" ,L"[image]",L"[/image]",L"int",L"char",L"if",L"for",L"while",
L"do",L"#include",L"#define",L"_asm",L"wchar_t",L"size_t",L"unsigned",L"return",L"long",L"short",L"void",L"typedef",
L"#ifdef",L"#endif",L"#ifndef",L"#if",L"string",L"using",L"namespace",L"public",L"private",L"protected",L"virtual",
L"static",L"internal",L"extern",L"new", L"this", L"ref", L"object" };//Size 39

using namespace Just_Editor;

using namespace Platform;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

CodeEditor::CodeEditor()
{
	InitializeComponent();
	//this->Frame->Navigated += ref new NavigatedEventHandler(this, &CodeEditor::ThisFrame_Navigated);
}

void CodeEditor::ThisFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	
}


void Just_Editor::CodeEditor::CodeEditorBox_TextChanging(Windows::UI::Xaml::Controls::RichEditBox^ sender, Windows::UI::Xaml::Controls::RichEditBoxTextChangingEventArgs^ args)
{
	Undo_Button->IsEnabled = sender->Document->CanUndo();
	Redo_Button->IsEnabled = sender->Document->CanRedo();

	//Seeach IDentifier && Highlight
	int c = 39, sl;
	Windows::UI::Text::ITextRange^ searchRange = sender->Document->GetRange(0, Windows::UI::Text::TextConstants::MaxUnitCount);
	searchRange->Move(Windows::UI::Text::TextRangeUnit::Character, 0);
	Windows::UI::Text::ITextCharacterFormat^ charFormatting = searchRange->CharacterFormat;
	charFormatting->ForegroundColor = Windows::UI::Colors::Gray;
	while (--c >= 0)
	{
		auto ChangeColorText = ref new Platform::String(MyIDentifier[c]);
		//int len = (int)((RichEditBox^)sender)->PlaceholderText->Length();
		searchRange = ((RichEditBox^)sender)->Document->GetRange(0, Windows::UI::Text::TextConstants::MaxUnitCount);
		searchRange->Move(Windows::UI::Text::TextRangeUnit::Character, 0);
		sl = searchRange->FindText(ChangeColorText, Windows::UI::Text::TextConstants::MaxUnitCount, Windows::UI::Text::FindOptions::Word);
		while (sl)
		{
			charFormatting = searchRange->CharacterFormat;
			charFormatting->ForegroundColor = c >= 4 ? Windows::UI::Colors::MediumBlue : Windows::UI::Colors::Red;
			sl = searchRange->FindText(ChangeColorText, Windows::UI::Text::TextConstants::MaxUnitCount, Windows::UI::Text::FindOptions::Word);
		}
	}
	
	if (thisWindowItem == nullptr)
		return;

	
	thisWindowItem->SetChanged(true);
}


void Just_Editor::CodeEditor::CodeEditorBox_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	if (e->Key == Windows::System::VirtualKey::Tab)
	{
		((RichEditBox^)sender)->Document->Selection->Text += "    ";
		((RichEditBox^)sender)->Document->Selection->MoveRight(Windows::UI::Text::TextRangeUnit::Character, 1, false);
	}
}


void Just_Editor::CodeEditor::Undo_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (CodeEditorBox->Document->CanUndo())
	{
		CodeEditorBox->Document->Undo();
	}
}


void Just_Editor::CodeEditor::Redo_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (CodeEditorBox->Document->CanRedo())
	{
		CodeEditorBox->Document->Redo();
	}
}

/*
void Just_Editor::CodeEditor::CodeEditorBox_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (thisWindowItem == nullptr)
		return;

	String^ thisText = "";
	CodeEditorBox->Document->GetText(Windows::UI::Text::TextGetOptions::None, &thisText);
	if (thisWindowItem->OriginalText == nullptr)
	{
		thisWindowItem->OriginalText = thisText;
	}
	thisWindowItem->SetChanged(thisText != thisWindowItem->OriginalText);
}
*/

void Just_Editor::CodeEditor::Save_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (this->thisWindowItem->ItemFile != nullptr)
	{
		Editor_Tools::WriteFile(this->thisWindowItem->ItemFile, GetEditBoxText());
		this->thisWindowItem->SetChanged(false);
	}
}
