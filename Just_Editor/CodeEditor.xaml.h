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
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CodeEditor sealed
	{
	public:
		CodeEditor();

		property DuronWindowItemxaml^ thisWindowItem;

		property bool isSmartDetectEnabled;
		property bool isHighlightEnabled;

//		void ThisFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);

		Platform::String^ GetEditBoxText()
		{
			Platform::String^ thisText = "";
			CodeEditorBox->Document->GetText(Windows::UI::Text::TextGetOptions::None, &thisText);
			return thisText;
		}

		Windows::UI::Text::ITextRange^ GetWordFromSelection(int SelectionIndex)
		{
			auto thisRange = CodeEditorBox->Document->GetRange(0, SelectionIndex + 10);
			auto wholeWord = thisRange->Text->Data();
			size_t WordLength = wcslen(wholeWord);
			bool CanAddBackLength = true, CanAddFrontLength = true;
			thisRange->EndPosition = SelectionIndex;
			do
			{
				if (CanAddBackLength)
				{
					CanAddBackLength = SelectionIndex < thisRange->EndPosition && wholeWord[thisRange->EndPosition] >= L'a' && wholeWord[thisRange->EndPosition] <= L'z';
					if (CanAddBackLength)
						thisRange->EndPosition++;
				}
				if (CanAddFrontLength)
				{
					thisRange->StartPosition = SelectionIndex;
				}
				CanAddFrontLength = --SelectionIndex >= 0 && wholeWord[SelectionIndex] >= L'a' && wholeWord[SelectionIndex] <= L'z';
			} while (CanAddFrontLength || CanAddBackLength);
			return thisRange;
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
};
}
