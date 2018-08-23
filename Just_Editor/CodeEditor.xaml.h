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
			SelectionIndex = thisRange->StartPosition;
			//auto FindResult = Editor_Tools::FindAllStr(wholeWord, L"//");
			if (!thisData->isHighlightEnabled)
				return thisRange;
			
			int CommentsNum = CommentList->Size;
			Windows::UI::Text::ITextRange^ thisCommentRange;
			while (--CommentsNum >= 0)
			{
				thisCommentRange = CommentList->GetAt(CommentsNum);
				if (wordLength < thisCommentRange->EndPosition)
				{
					CommentList->RemoveAt(CommentsNum);
				}
				else
				{
					auto thisCommentText = thisCommentRange->Text->Data();
					if (SelectionIndex < thisCommentRange->EndPosition && SelectionIndex >= thisCommentRange->StartPosition)
					{
						AutoDetect(thisCommentRange->StartPosition, thisCommentRange->EndPosition + thisRange->EndPosition , true);
						CommentList->RemoveAt(CommentsNum);
						thisRange->StartPosition = thisRange->EndPosition;
						//return thisRange;
					}
				}
			}
			try
			{
				while (--SelectionIndex > 0 && wholeWord[SelectionIndex] != L'\r');
			}
			catch (Platform::Exception^)
			{
				return thisRange;
			}
			CommentsNum = (int)Editor_Tools::FindStr(wholeWord, L"/", wordLength, 1, SelectionIndex);
			CanAddFrontLength = wholeWord[CommentsNum + 1] == L'/';
			if (CommentsNum != -1 && CommentsNum < thisRange->EndPosition)
			{
				if (CanAddFrontLength || wholeWord[CommentsNum + 1] == L'*')
				{
					if (CanAddFrontLength)
					{
						SelectionIndex = (int)Editor_Tools::FindStr(wholeWord, L"\r", wordLength, 1, CommentsNum);
					}
					else
					{
						SelectionIndex = (int)Editor_Tools::FindStr(wholeWord, L"*/", wordLength, 2, CommentsNum);
					}
					if (SelectionIndex != -1)
					{
						if (!CanAddFrontLength)
						{
							SelectionIndex += 2;
							CommentList->Append(thisCommentRange);
						}
						else
							thisRange->EndPosition = thisRange->StartPosition;
						
						thisCommentRange = CodeEditorBox->Document->GetRange(CommentsNum, SelectionIndex);
						thisCommentRange->CharacterFormat->ForegroundColor =
							Windows::UI::Colors::DarkSeaGreen;
					}
				}
				else if (CommentsNum && wholeWord[CommentsNum - 1] == L'*')
				{
					while (wholeWord[++SelectionIndex] != L'\0' && wholeWord[SelectionIndex] != L'\r');
					do
					{
						CanAddFrontLength = !(wholeWord[SelectionIndex] == L'*' && wholeWord[SelectionIndex - 1] == L'/');
					} while (--SelectionIndex > 0 && CanAddFrontLength);
					if (!CanAddBackLength)
					{
						thisCommentRange = CodeEditorBox->Document->GetRange(SelectionIndex, CommentsNum + 1);
						thisCommentRange->CharacterFormat->ForegroundColor = Windows::UI::Colors::DarkSeaGreen;
						CommentList->Append(thisCommentRange);
					}
					else
						AutoDetect(SelectionIndex, CommentsNum + 1, true);
				}
				else if (!CanAddFrontLength)
				{
					SelectionIndex = CommentsNum;
					while (wholeWord[++SelectionIndex] != L'\0' && wholeWord[SelectionIndex] != L'\r');
					AutoDetect(CommentsNum, SelectionIndex, false);
				}
			}
			else if (CommentsNum == -1 || !CanAddFrontLength)
			{
				CommentsNum = SelectionIndex;
				while (wholeWord[++SelectionIndex] != L'\0' && wholeWord[SelectionIndex] != L'\r');
				AutoDetect(CommentsNum, SelectionIndex, false);
			}
			if (!thisData->isSmartDetectEnabled)
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
		Platform::Collections::Vector<Windows::UI::Text::ITextRange^>^ CommentList;

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
