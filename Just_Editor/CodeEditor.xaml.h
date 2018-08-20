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

		Windows::UI::Text::ITextRange^ GetWordFromSelection(int SelectionIndex)
		{
			auto thisRange = CodeEditorBox->Document->GetRange(0, Windows::UI::Text::TextConstants::MaxUnitCount);
			auto wholeWord = thisRange->Text->Data();
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
					CanAddBackLength = SelectionIndex < thisRange->EndPosition && wholeWord[thisRange->EndPosition] != '\r' && 
						((wholeWord[thisRange->EndPosition] >= L'a' && wholeWord[thisRange->EndPosition] <= L'z') ||
						(wholeWord[SelectionIndex] >= L'A' && wholeWord[SelectionIndex] <= L'Z' || wholeWord[thisRange->EndPosition] == L'#'));
					if (CanAddBackLength)
						thisRange->EndPosition++;
				}
			}
			/*
			SelectionIndex = thisRange->StartPosition;
			while (SelectionIndex-- > 0 && wholeWord[SelectionIndex] != L'\r')
			{
				if (wholeWord[SelectionIndex] == L'*' && wholeWord[SelectionIndex - 1] == L'/')
				{
					thisRange->StartPosition = SelectionIndex - 1;
					SelectionIndex = thisRange->EndPosition;
					while (wholeWord[SelectionIndex] != L'*' && wholeWord[SelectionIndex + 1] != L'/' && wholeWord[SelectionIndex] != L'\0')
					{
						thisRange->EndPosition = SelectionIndex;
						SelectionIndex++;
					}
					thisRange->EndPosition += 3;
					thisRange->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
					thisRange->StartPosition = thisRange->EndPosition;
					break;
				}
			}*/
			SelectionIndex = thisRange->StartPosition;
			while (--SelectionIndex > 0 && wholeWord[SelectionIndex] != L'\r')
			{
				if (wholeWord[SelectionIndex] == L'/' && wholeWord[SelectionIndex - 1] == L'/')
				{
					thisRange->StartPosition = SelectionIndex - 1;
					SelectionIndex = thisRange->EndPosition;
					while (wholeWord[SelectionIndex] != L'\r' && wholeWord[SelectionIndex] != L'\0')
					{
						thisRange->EndPosition = SelectionIndex;
						SelectionIndex++;
					}
					thisRange->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
					thisRange->StartPosition = thisRange->EndPosition;
					break;
				}
			}
			
			return thisRange;
		}

		void UpdateBindings()
		{
			this->Bindings->Update();
			AutoDetect();
		}

		void AutoDetect();

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
