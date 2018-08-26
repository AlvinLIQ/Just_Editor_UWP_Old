//
// CodeEditor.xaml.cpp
// Implementation of the CodeEditor class
//

#include "pch.h"
#include "CodeEditor.xaml.h"
#include "DuronSmartDetect.xaml.h"
#include "CaesarPanel.xaml.h"

bool isCtrlHeld = false, isGridCtrlHeld = false;

using namespace Just_Editor;

using namespace Platform;
using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;

String^ MyIdentifierArray[] = {L"//", L"/*", L"Vector", L"Task" ,L"Array",L"Enum",L"int",L"char",L"if",L"for",L"while",
L"do",L"#include",L"#define",L"_asm",L"wchar_t",L"size_t",L"unsigned",L"return",L"long",L"short",L"void",L"typedef",
L"#ifdef",L"#endif",L"#ifndef",L"#if",L"string",L"using",L"namespace",L"public",L"private",L"protected",L"virtual",
L"static",L"internal",L"extern",L"new", L"this", L"ref", L"object", L"bool", L"selead", L"var", L"auto" };
const wchar_t EndChar[] = L"\r*/";
Point thisPoint;
int LineNum;
// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

CodeEditor::CodeEditor()
{
	InitializeComponent();
	LineNum = 0;
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

void Just_Editor::CodeEditor::AutoDetect(int StartIndex, int EndIndex, bool isExtend)
{
	Windows::UI::Text::ITextRange^ searchRange = CodeEditorBox->Document->GetRange(StartIndex, EndIndex);
	searchRange->CharacterFormat->ForegroundColor = thisData->Editor_SymbolColor;
	if (!thisData->isHighlightEnabled)
		return;
	int c = IdentifierNum + 2, sl;
	auto thisChar = std::wstring(searchRange->Text->Data());
	int End;
	while (--c >= 0)
	{
		//int len = (int)((RichEditBox^)sender)->PlaceholderText->Length();
		sl = searchRange->FindText(MyIdentifierArray[c], StartIndex ? searchRange->Length : EndIndex, c > 1 ? Windows::UI::Text::FindOptions::Word : Windows::UI::Text::FindOptions::None);
		while (sl)
		{
			if (c > 1)
			{
				searchRange->CharacterFormat->ForegroundColor = c > 5 ? 
					thisData->IdentifierHighlightColor :
					thisData->Editor_ForegroundBrush->Color;
			}
			else
			{
				End = searchRange->EndPosition;
				sl = StartIndex ? 0 : searchRange->EndPosition;
				while (thisChar[sl] != L'\0' && ((thisChar[sl] != EndChar[c] )
					|| (c && !(thisChar[sl] == EndChar[c] && thisChar[sl + 1] == EndChar[c + 1]))))
				{
					searchRange->EndPosition++;
					sl++;
				}
				if (c)
				{
					if (thisChar[sl] != L'\0')
					{
						if (isExtend)
							searchRange->EndPosition += 2;
						searchRange->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
					}
				}
				else
				{
					searchRange->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
				}

				
				searchRange->EndPosition = End;
			}
			sl = searchRange->FindText(MyIdentifierArray[c], StartIndex ? searchRange->Length : EndIndex, c > 1 ? Windows::UI::Text::FindOptions::Word : Windows::UI::Text::FindOptions::None);
		}
		searchRange = CodeEditorBox->Document->GetRange(StartIndex, EndIndex);
	}
}

Windows::UI::Text::ITextRange^ Just_Editor::CodeEditor::GetWordFromSelection(int SelectionIndex)
{
	auto thisRange = CodeEditorBox->Document->GetRange(0, Windows::UI::Text::TextConstants::MaxUnitCount);
	auto wholeWord = thisRange->Text->Data();
	auto wordLength = thisRange->EndPosition;
	bool CanAddBackLength = true, CanAddFrontLength = true;
	thisRange->EndPosition = SelectionIndex;
	while (CanAddFrontLength || CanAddBackLength)
	{
		if (CanAddFrontLength)
		{
			thisRange->StartPosition = SelectionIndex;
			CanAddFrontLength = --SelectionIndex >= 0 && ((wholeWord[SelectionIndex] >= L'a' && wholeWord[SelectionIndex] <= L'z') ||
				(wholeWord[SelectionIndex] >= L'A' && wholeWord[SelectionIndex] <= L'Z') || wholeWord[SelectionIndex] == L'#');
		}
		if (CanAddBackLength)
		{
			CanAddBackLength = SelectionIndex < thisRange->EndPosition &&
				((wholeWord[thisRange->EndPosition] >= L'a' && wholeWord[thisRange->EndPosition] <= L'z') ||
				(wholeWord[thisRange->EndPosition] >= L'A' && wholeWord[thisRange->EndPosition] <= L'Z' || wholeWord[thisRange->EndPosition] == L'#'));
			if (CanAddBackLength)
				thisRange->EndPosition++;
		}
	}
	//auto FindResult = Editor_Tools::FindAllStr(wholeWord, L"//");
	if (!thisData->isHighlightEnabled)
		return thisRange;
	
	int EndIndex;
	if (thisData->isLineNumEnabled)
	{
		EndIndex = Editor_Tools::GetStrNum(wholeWord, L"\r", wordLength, 1);
		if (EndIndex > LineNum)
		{
			do
			{
				CodeEditorBox->Header += (++LineNum).ToString() + L'\r';
			} while (EndIndex > LineNum);
		}
		else if (EndIndex < LineNum)
		{
			auto LineNums = CodeEditorBox->Header->ToString()->Data();
			SelectionIndex = (int)wcslen(LineNums);
			while (EndIndex < LineNum && --SelectionIndex > 0)
			{
				if (LineNums[SelectionIndex - 1] == L'\r')
					LineNum--;
			}
			CodeEditorBox->Header = ref new String(Editor_Tools::SubStr(LineNums, 0, SelectionIndex));
		}
	}

	EndIndex = thisRange->EndPosition - 1;
	while (wholeWord[++EndIndex] != L'\0' && wholeWord[EndIndex] != L'\r');
	SelectionIndex = EndIndex;
	if (SelectionIndex)
	{
		while (--SelectionIndex >= 0 && wholeWord[SelectionIndex] != L'\r' && wholeWord[SelectionIndex] != L'/');
		if (SelectionIndex && wholeWord[SelectionIndex - 1] == L'/')
			SelectionIndex -= 1;
	}
	int FindIndex = wholeWord[SelectionIndex] == L'/' ? SelectionIndex : -1;
	if (FindIndex != -1)
	{
		if (wholeWord[FindIndex + 1] == L'*')
		{
			while (wholeWord[++SelectionIndex] != L'\0' && !(wholeWord[SelectionIndex] == L'*' && wholeWord[SelectionIndex + 1] == L'/'));
			if (wholeWord[SelectionIndex] != L'\0')
			{
				auto commentRange = CodeEditorBox->Document->GetRange(FindIndex, SelectionIndex + 2);
				commentRange->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
				CodeEditorBox->Document->Selection->CharacterFormat->ForegroundColor = thisData->Editor_SymbolColor;
			}
		}
		else
		{
			CanAddBackLength = FindIndex > 0;
			if (CanAddBackLength && wholeWord[FindIndex - 1] == L'*')
			{
				SelectionIndex = FindIndex - 1;
				while (--SelectionIndex && !(wholeWord[SelectionIndex] == L'*' && wholeWord[SelectionIndex - 1] == L'/'));
				if (wholeWord[SelectionIndex] == L'*' && wholeWord[SelectionIndex - 1] == L'/')
				{
					auto commentRange = CodeEditorBox->Document->GetRange(SelectionIndex - 1, FindIndex + 1);
					commentRange->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
					CodeEditorBox->Document->Selection->CharacterFormat->ForegroundColor = thisData->Editor_SymbolColor;
				}
			}
			else if (wholeWord[FindIndex + 1] == L'/')
			{
				CodeEditorBox->Document->GetRange(FindIndex, EndIndex)->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
			}
			else if (!(CanAddBackLength && wholeWord[FindIndex - 1] == L'/'))
			{
				SelectionIndex = EndIndex;
				while (wholeWord[++SelectionIndex] != L'\0' && !(wholeWord[SelectionIndex] == L'*' && wholeWord[SelectionIndex + 1] == L'/'));
				if (wholeWord[SelectionIndex] == L'*' && wholeWord[SelectionIndex + 1] == L'/')
				{
					EndIndex = SelectionIndex + 2;
				}
				AutoDetect(FindIndex, EndIndex, true);
			}
		}
	}
	else
	{
		FindIndex = (int)Editor_Tools::FindStr(wholeWord, L"*", EndIndex, 1, (size_t)SelectionIndex);
		if (FindIndex != -1)
		{
			while (--SelectionIndex > 0 && !(wholeWord[SelectionIndex] == L'*' && wholeWord[SelectionIndex - 1] == L'/'));
			if (SelectionIndex <= 0)
				SelectionIndex = 1;
			if (wholeWord[SelectionIndex] == L'*' && wholeWord[SelectionIndex - 1] == L'/')
				AutoDetect(SelectionIndex - 1, EndIndex, false);
		}
	}
	if (!thisData->isSmartDetectEnabled || thisRange->CharacterFormat->ForegroundColor.G == Windows::UI::Colors::DarkSeaGreen.G)
		thisRange->EndPosition = thisRange->StartPosition;
	return thisRange;
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
		if (isCtrlHeld)
		{
			e->Handled = true;
			SearchInRange(CodeEditorBox->Document->GetRange(CodeEditorBox->Document->Selection->EndPosition, Windows::UI::Text::TextConstants::MaxUnitCount));
		}
		else
		{
			CodeEditorBox->Document->Selection->Text += "\t";
			CodeEditorBox->Document->Selection->StartPosition = CodeEditorBox->Document->Selection->EndPosition;
			if (CodeEditorBox->Document->Selection->Length == 0 && !(CodeEditorBox->Document->Selection->CharacterFormat->ForegroundColor.R == Windows::UI::Colors::DarkSeaGreen.R &&
				CodeEditorBox->Document->Selection->CharacterFormat->ForegroundColor.G == Windows::UI::Colors::DarkSeaGreen.G &&
				CodeEditorBox->Document->Selection->CharacterFormat->ForegroundColor.B == Windows::UI::Colors::DarkSeaGreen.B))
				CodeEditorBox->Document->Selection->CharacterFormat->ForegroundColor = thisData->Editor_SymbolColor;
		}
	}
	else if (isCtrlHeld && e->Key == Windows::System::VirtualKey::Space && !SmartDetect->ActualWidth)
	{
		e->Handled = true;
		SearchInRange(CodeEditorBox->Document->GetRange(0, CodeEditorBox->Document->Selection->StartPosition));
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
			bool isClass = SmartDetect->SelectedItem->isClass;
			SmartDetect->wordRange->Text += Editor_Tools::SubPStr(SmartDetect->SelectedItem->Identifier, SmartDetect->StartIndex);
			
			if (isClass)
			{
				SmartDetect->wordRange->CharacterFormat->ForegroundColor = thisData->Editor_ForegroundBrush->Color;
			}
			SmartDetect->SelectedItem = nullptr;
			SmartDetect->Width = 0;
			SmartDetect->wordRange->MatchSelection();
		}
		else
		{
			CodeEditorBox->Document->Selection->Text += L"\r\n";
		}

		((RichEditBox^)sender)->Document->Selection->MoveRight(Windows::UI::Text::TextRangeUnit::Character, 1, false);
	}
	else if(CodeEditorBox->Document->Selection->Length == 0)
	{
		if (!(CodeEditorBox->Document->Selection->CharacterFormat->ForegroundColor.R == Windows::UI::Colors::DarkSeaGreen.R &&
			CodeEditorBox->Document->Selection->CharacterFormat->ForegroundColor.G == Windows::UI::Colors::DarkSeaGreen.G &&
			CodeEditorBox->Document->Selection->CharacterFormat->ForegroundColor.B == Windows::UI::Colors::DarkSeaGreen.B))
			CodeEditorBox->Document->Selection->CharacterFormat->ForegroundColor = thisData->Editor_SymbolColor;
	}
	isCtrlHeld = e->Key == Windows::System::VirtualKey::Control;
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
	if (isGridCtrlHeld)
	{
		if (e->Key == Windows::System::VirtualKey::S)
			SaveFile();
		else if (e->Key == Windows::System::VirtualKey::F)
			Search_BoxShowHide(true);
	}
	isGridCtrlHeld = (e->Key == Windows::System::VirtualKey::Control);
}


void Just_Editor::CodeEditor::Caesar_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (MainGrid->Children->Size > 2)
	{
		ExtraColumn->Width = 0;
		auto thisItem = MainGrid->Children->GetAt(2);
		MainGrid->Children->RemoveAtEnd();
		delete[] thisItem;
	}
	else
	{
		auto thisPanel = ref new CaesarPanel;
		//thisPanel->SetPanelMode(0);//Caesar

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
	SmartDetect->DetectWordFromStrArray(GetWordFromSelection(CodeEditorBox->Document->Selection->EndPosition), thisData->isHighlightEnabled);
	if (thisData->isSmartDetectEnabled)
	{
		if (SmartDetect->ActualHeight)
		{
			CodeEditorBox->Document->Selection->GetPoint(Windows::UI::Text::HorizontalCharacterAlignment::Right,
				Windows::UI::Text::VerticalCharacterAlignment::Baseline, Windows::UI::Text::PointOptions::ClientCoordinates, &thisPoint);
			//auto thisTransform = ref new TranslateTransform;
			thisPoint.X += 40;
			Pipe_Trans->X = thisPoint.X + SmartDetect->Width > CodeEditorBox->RenderSize.Width ? CodeEditorBox->RenderSize.Width - SmartDetect->Width : thisPoint.X;
			CodeEditorBox->Document->Selection->GetPoint(Windows::UI::Text::HorizontalCharacterAlignment::Right,
				Windows::UI::Text::VerticalCharacterAlignment::Bottom, Windows::UI::Text::PointOptions::NoVerticalScroll, &thisPoint);
			thisPoint.Y -= Window::Current->CoreWindow->Bounds.Top + 55;
			Pipe_Trans->Y = thisPoint.Y + SmartDetect->Height > CodeEditorBox->RenderSize.Height ? CodeEditorBox->RenderSize.Height - SmartDetect->Height : thisPoint.Y;
		}
	}
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
		Replace_Box->Foreground = Redo_Button->Foreground;
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
		Replace_Box->Foreground = Redo_Button->Foreground;
		CodeEditorBox->Document->Selection->StartPosition = searchRange->StartPosition;
		CodeEditorBox->Document->Selection->EndPosition = searchRange->EndPosition;
		CodeEditorBox->Focus(Windows::UI::Xaml::FocusState::Pointer);
		Replace_Box->IsEnabled = true;
		Replace_Row->Height = GridLength(1, GridUnitType::Auto);
		SmartDetect->Width = 0;
		SmartDetect->SelectedItem = nullptr;
	}
	else
		Replace_Box->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Red);
}

void Just_Editor::CodeEditor::MainGrid_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	if (isCtrlHeld)
		isCtrlHeld = !(e->Key == Windows::System::VirtualKey::Control);
}


void Just_Editor::CodeEditor::Hide_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Search_BoxShowHide(false);
}


void Just_Editor::CodeEditor::ContentElement_ViewChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs^ e)
{
	((ContentPresenter^)((Grid^)((ScrollViewer^)sender)->Parent)->Children->GetAt(2))->Margin = Thickness(0, -((ScrollViewer^)sender)->VerticalOffset, 0, 8);
}


void Just_Editor::CodeEditor::ContentElement_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}
