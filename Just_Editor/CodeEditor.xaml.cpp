//
// CodeEditor.xaml.cpp
// Implementation of the CodeEditor class
//

#include "pch.h"
#include "CodeEditor.xaml.h"
#include "DuronSmartDetect.xaml.h"
#include "CaesarPanel.xaml.h"

bool isCtrlHeld = false;

using namespace Just_Editor;

using namespace Platform;
using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;

String^ MyIdentifierArray[] = { L"[code]", L"[/code]" ,L"[image]",L"[/image]",L"int",L"char",L"if",L"for",L"while",
L"do",L"#include",L"#define",L"_asm",L"wchar_t",L"size_t",L"unsigned",L"return",L"long",L"short",L"void",L"typedef",
L"#ifdef",L"#endif",L"#ifndef",L"#if",L"string",L"using",L"namespace",L"public",L"private",L"protected",L"virtual",
L"static",L"internal",L"extern",L"new", L"this", L"ref", L"object", L"bool", L"selead", L"var", L"auto"};
Point thisPoint;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

CodeEditor::CodeEditor()
{
	InitializeComponent();
	if (!Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
		CodeEditorBox->PreviewKeyDown += ref new Windows::UI::Xaml::Input::KeyEventHandler(this, &Just_Editor::CodeEditor::CodeEditorBox_KeyDown);
	else
	{
		ToolBar_Row->Height = 35;
		CodeEditorBox->KeyDown += ref new Windows::UI::Xaml::Input::KeyEventHandler(this, &Just_Editor::CodeEditor::CodeEditorBox_KeyDown);
	}

	CodeEditorBox->Document->UndoLimit = MaxUndoLimit;
	CodeEditorBox->Document->BeginUndoGroup();
	//this->Frame->Navigated += ref new NavigatedEventHandler(this, &CodeEditor::ThisFrame_Navigated);
}
/*
void CodeEditor::ThisFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{

}
*/

void Just_Editor::CodeEditor::AutoDetect()
{
	if (!isHighlightEnabled)
		return;
	int c = IdentifierNum, sl;
	Windows::UI::Text::ITextRange^ searchRange = CodeEditorBox->Document->GetRange(0, Windows::UI::Text::TextConstants::MaxUnitCount);
	searchRange->Move(Windows::UI::Text::TextRangeUnit::Character, 0);
	Windows::UI::Text::ITextCharacterFormat^ charFormatting = searchRange->CharacterFormat;
	charFormatting->ForegroundColor = Windows::UI::Colors::Gray;
	while (--c >= 0)
	{
		//int len = (int)((RichEditBox^)sender)->PlaceholderText->Length();
		searchRange = CodeEditorBox->Document->GetRange(0, Windows::UI::Text::TextConstants::MaxUnitCount);
		searchRange->Move(Windows::UI::Text::TextRangeUnit::Character, 0);
		sl = searchRange->FindText(MyIdentifierArray[c], Windows::UI::Text::TextConstants::MaxUnitCount, Windows::UI::Text::FindOptions::Word);
		while (sl)
		{
			charFormatting = searchRange->CharacterFormat;
			charFormatting->ForegroundColor = Windows::UI::Colors::MediumBlue;
			sl = searchRange->FindText(MyIdentifierArray[c], Windows::UI::Text::TextConstants::MaxUnitCount, Windows::UI::Text::FindOptions::Word);
		}
	}
}

void Just_Editor::CodeEditor::ChangeTextColor(Platform::String^ thisIDentifier, Windows::UI::Text::ITextRange^ searchRange, Windows::UI::Color foreColor, Windows::UI::Color backColor)
{
	int sl = searchRange->FindText(thisIDentifier, Windows::UI::Text::TextConstants::MaxUnitCount, Windows::UI::Text::FindOptions::Word);
	while (sl)
	{
		searchRange->CharacterFormat->BackgroundColor = backColor;
		searchRange->CharacterFormat->ForegroundColor = foreColor;
		sl = searchRange->FindText(thisIDentifier, Windows::UI::Text::TextConstants::MaxUnitCount, Windows::UI::Text::FindOptions::Word);
	}
}

void Just_Editor::CodeEditor::CodeEditorBox_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	if (e->Key == Windows::System::VirtualKey::Tab)
	{
		((RichEditBox^)sender)->Document->Selection->Text += "    ";
		((RichEditBox^)sender)->Document->Selection->MoveRight(Windows::UI::Text::TextRangeUnit::Character, 1, false);
	}
	else if (e->Key == Windows::System::VirtualKey::Escape)
	{
		if (SmartDetect->Width)
		{
			SmartDetect->Width = 0;
			SmartDetect->SelectedItem = nullptr;
		}
		if (Search_Grid->Width)
		{
			Search_BoxShowHide(false);
		}
	}
	else if (e->Key == Windows::System::VirtualKey::Up && SmartDetect->SelectedItem != nullptr)
	{
		e->Handled = true;
		SmartDetect->SelectAt(SmartDetect->SelectedItem->ItemIndex - 1);
	}
	else if (e->Key == Windows::System::VirtualKey::Down && SmartDetect->SelectedItem != nullptr)
	{
		e->Handled = true;
		SmartDetect->SelectAt(SmartDetect->SelectedItem->ItemIndex + 1);
	}
	else if (e->Key == Windows::System::VirtualKey::Enter)
	{
		e->Handled = true;
		if (SmartDetect->SelectedItem != nullptr)
		{
			SmartDetect->wordRange->Text += Editor_Tools::SubPStr(SmartDetect->SelectedItem->Identifier, SmartDetect->StartIndex);
			SmartDetect->wordRange->MatchSelection();
			if (isHighlightEnabled)
				SmartDetect->wordRange->CharacterFormat->ForegroundColor =  Windows::UI::Colors::MediumBlue;
			SmartDetect->SelectedItem = nullptr;
			SmartDetect->Width = 0;
		}
		else
		{
			CodeEditorBox->Document->Selection->Text += L"\r\n";
		}

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
	SaveFile();
}


void Just_Editor::CodeEditor::MainGrid_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	if (isCtrlHeld)
	{
		if (e->Key == Windows::System::VirtualKey::S)
			SaveFile();
		else if (e->Key == Windows::System::VirtualKey::F)
			Search_BoxShowHide(!Search_Grid->Width);
		else if (e->Key == Windows::System::VirtualKey::Q)
			SearchInRange(CodeEditorBox->Document->GetRange(0, CodeEditorBox->Document->Selection->StartPosition));
		else if (e->Key == Windows::System::VirtualKey::E)
			SearchInRange(CodeEditorBox->Document->GetRange(CodeEditorBox->Document->Selection->EndPosition, Windows::UI::Text::TextConstants::MaxUnitCount));
	}
	isCtrlHeld = (e->Key == Windows::System::VirtualKey::Control);

}


void Just_Editor::CodeEditor::Caesar_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (MainGrid->Children->Size > 2)
	{
		ExtraColumn->Width = 0;
		auto thisItem = MainGrid->Children->GetAt(3);
		MainGrid->Children->RemoveAtEnd();
		delete[] thisItem;
	}
	else
	{
		auto thisPanel = ref new CaesarPanel;
		thisPanel->SetPanelMode(0);//Caesar

		MainGrid->Children->Append(thisPanel);

		MainGrid->SetColumn(thisPanel, 2);
		MainGrid->SetRow(thisPanel, 1);
		ExtraColumn->Width = MainGrid->ActualWidth > 600 ? MainGrid->ActualWidth / 2 : MainGrid->ActualWidth;

	}
}


void Just_Editor::CodeEditor::MainGrid_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	if (ExtraColumn->Width.Value)
	{
		ExtraColumn->Width = e->NewSize.Width > 600 ? e->NewSize.Width / 2 : MainGrid->ActualWidth;
	}
}


void Just_Editor::CodeEditor::CodeEditorBox_TextChanging(Windows::UI::Xaml::Controls::RichEditBox^ sender, Windows::UI::Xaml::Controls::RichEditBoxTextChangingEventArgs^ args)
{
	Undo_Button->IsEnabled = CodeEditorBox->Document->CanUndo();
	Redo_Button->IsEnabled = CodeEditorBox->Document->CanRedo();

	if (thisWindowItem == nullptr)
		return;

	if (!thisWindowItem->isChanged)
	{
		thisWindowItem->SetChanged(1);
	}

	if (isSmartDetectEnabled)
	{
		SmartDetect->DetectWordFromStrArray(GetWordFromSelection(CodeEditorBox->Document->Selection->EndPosition), isHighlightEnabled);

		if (SmartDetect->ActualHeight)
		{
			CodeEditorBox->Document->Selection->GetPoint(Windows::UI::Text::HorizontalCharacterAlignment::Right,
				Windows::UI::Text::VerticalCharacterAlignment::Bottom, Windows::UI::Text::PointOptions::ClientCoordinates, &thisPoint);
			//auto thisTransform = ref new TranslateTransform;
			thisPoint.X += 10;
			Pipe_Trans->X = CodeEditorBox->ActualWidth >= thisPoint.X + SmartDetect->Width ? thisPoint.X : CodeEditorBox->ActualWidth - SmartDetect->Width;

			CodeEditorBox->Document->Selection->GetPoint(Windows::UI::Text::HorizontalCharacterAlignment::Right,
				Windows::UI::Text::VerticalCharacterAlignment::Bottom, Windows::UI::Text::PointOptions::NoVerticalScroll, &thisPoint);
			thisPoint.Y -= Window::Current->CoreWindow->Bounds.Top + 55;
			Pipe_Trans->Y = thisPoint.Y + SmartDetect->Height > CodeEditorBox->RenderSize.Height ? CodeEditorBox->RenderSize.Height - SmartDetect->Height : thisPoint.Y;
		}
	}
	else
		AutoDetect();
}


void Just_Editor::CodeEditor::Search_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Search_BoxShowHide(!Search_Grid->Width);
}

void Just_Editor::CodeEditor::Search_BoxShowHide(bool isShow)
{
	if (!isShow)
	{
		Search_Grid->Width = 0;
		Replace_Box->IsEnabled = false;
		Replace_Row->Height = 0;
	}
	else
	{
		Search_Box->Text = "";
		Search_Grid->Width = 180;
		Search_Box->Focus(Windows::UI::Xaml::FocusState::Keyboard);
	}
}

void Just_Editor::CodeEditor::Search_Box_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	auto thisTextBox = dynamic_cast<TextBox^>(sender);
	if (e->Key == Windows::System::VirtualKey::Enter)
	{
		if (Search_Box == thisTextBox)
		{
			SearchInRange(CodeEditorBox->Document->GetRange(CodeEditorBox->Document->Selection->EndPosition, Windows::UI::Text::TextConstants::MaxUnitCount));
		}
		else if (Replace_Box == thisTextBox && CodeEditorBox->Document->Selection->EndPosition != CodeEditorBox->Document->Selection->StartPosition)
		{
			CodeEditorBox->Document->Selection->Text = Replace_Box->Text;
		}
	}
	else if (e->Key == Windows::System::VirtualKey::Escape)
	{
		Search_BoxShowHide(false);
		CodeEditorBox->Focus(Windows::UI::Xaml::FocusState::Pointer);
	}
}

void Just_Editor::CodeEditor::SearchInRange(Windows::UI::Text::ITextRange^ searchRange)
{
	if (Search_Box->Text == "")
		return;

	if (searchRange->FindText(Search_Box->Text, searchRange->EndPosition, Windows::UI::Text::FindOptions::None))
	{
		CodeEditorBox->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::Gray);
		CodeEditorBox->Document->Selection->StartPosition = searchRange->StartPosition;
		CodeEditorBox->Document->Selection->EndPosition = searchRange->EndPosition;
		CodeEditorBox->Focus(Windows::UI::Xaml::FocusState::Pointer);
		Replace_Box->IsEnabled = true;
		Replace_Row->Height = GridLength(1, GridUnitType::Auto);
	}
	else
		CodeEditorBox->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::Red);
}