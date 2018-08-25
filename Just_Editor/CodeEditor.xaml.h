//
// CodeEditor.xaml.h
// Declaration of the CodeEditor class
//

#pragma once

#include "CodeEditor.g.h"
#include "DuronWindowItemxaml.xaml.h"
#include "Editor_Tools.h"

namespace Just_Editor
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::UI::Xaml::Data::Bindable]
	public ref class CodeEditor sealed
	{
	public:
		CodeEditor();

		property DuronWindowItemxaml^ thisWindowItem;

		property Editor_Data^ thisData;

//		void ThisFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);

		Platform::String^ GetEditBoxText()
		{
			Platform::String^ thisText = "";
			CodeEditorBox->Document->GetText(Windows::UI::Text::TextGetOptions::UseCrlf, &thisText);
			return thisText;
		}

		Windows::UI::Xaml::Controls::RichEditBox^ GetEditBox()
		{
			return CodeEditorBox;
		}

		Windows::UI::Text::ITextRange^ GetWordFromSelection(int SelectionIndex)
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
			
			int EndIndex = thisRange->EndPosition - 1;
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
						CodeEditorBox->Document->GetRange(FindIndex, SelectionIndex + 2)->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
					}
				}
				else if (FindIndex > 0)
				{
					if (wholeWord[FindIndex - 1] == L'*')
					{
						SelectionIndex = FindIndex - 1;
						while (--SelectionIndex && !(wholeWord[SelectionIndex] == L'*' && wholeWord[SelectionIndex - 1] == L'/'));
						if (wholeWord[SelectionIndex] == L'*' && wholeWord[SelectionIndex - 1] == L'/')
							CodeEditorBox->Document->GetRange(SelectionIndex - 1, FindIndex + 1)->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
					}
					else if (wholeWord[FindIndex - 1] == L'/')
						CodeEditorBox->Document->GetRange(FindIndex - 1, EndIndex - 1)->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
				}
				else
				{
					if (wholeWord[FindIndex + 1] == L'/')
						CodeEditorBox->Document->GetRange(FindIndex, EndIndex)->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
					else
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

		void UpdateBindings()
		{
			this->Bindings->Update();
			AutoDetect(0, Windows::UI::Text::TextConstants::MaxUnitCount, true);
		}

		void AutoDetect(int StartIndex, int EndIndex, bool isExtend);

		void SaveFile()
		{
			if (thisWindowItem->ItemFile != nullptr)
			{
				Editor_Tools::WriteFile(thisWindowItem->ItemFile, GetEditBoxText());
				thisWindowItem->SetChanged(false);
			}
			else
			{
				auto theFolderPicker = ref new Windows::Storage::Pickers::FolderPicker;
				theFolderPicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::Thumbnail;
				theFolderPicker->CommitButtonText = "Save";
				theFolderPicker->FileTypeFilter->Append("*");
				theFolderPicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::DocumentsLibrary;
				concurrency::create_task(theFolderPicker->PickSingleFolderAsync()).then([this](concurrency::task<Windows::Storage::StorageFolder^> thisTask)
				{
					try
					{
						Windows::Storage::StorageFolder^ thisFolder = thisTask.get();
						if (thisFolder == nullptr)
							return;

						concurrency::create_task(thisFolder->CreateFileAsync(thisWindowItem->FileName, Windows::Storage::CreationCollisionOption::ReplaceExisting)).then([this](concurrency::task<Windows::Storage::StorageFile^> thisTask)
						{
							try
							{
								auto thisFile = thisTask.get();
								this->thisWindowItem->ItemFile = thisFile;
								Editor_Tools::WriteFile(thisFile, GetEditBoxText());
								thisWindowItem->SetChanged(false);
							}
							catch (Platform::Exception^)
							{
								//Create File Error
							}
						});
					}
					catch (Platform::Exception^)
					{
						//Open File Error
					}
				}, concurrency::task_continuation_context::use_current());

			}
		}

		void Search_BoxShowHide(bool isShow);
	private:
		void CodeEditorBox_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void Undo_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Redo_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Save_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void MainGrid_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void ChangeTextColor(Platform::String^ thisIDentifier, Windows::UI::Text::ITextRange^ searchRange, Windows::UI::Color foreColor, Windows::UI::Color backColor);
		void Caesar_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void MainGrid_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		void CodeEditorBox_TextChanging(Windows::UI::Xaml::Controls::RichEditBox^ sender, Windows::UI::Xaml::Controls::RichEditBoxTextChangingEventArgs^ args);
		void Search_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Search_Box_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void SearchInRange(Windows::UI::Text::ITextRange^ searchRange);
		void MainGrid_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void Hide_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
};
}
