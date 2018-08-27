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
		property bool isDetecting;
		property int LineNum;

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

		Windows::UI::Text::ITextRange^ GetWordFromSelection(int SelectionIndex);

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
		void ContentElement_ViewChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs^ e);
		void CodeEditorBox_Paste(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextControlPasteEventArgs^ e);
		void HeaderContentPresenter_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
