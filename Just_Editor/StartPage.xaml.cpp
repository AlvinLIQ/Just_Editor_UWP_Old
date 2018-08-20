//
// StartPage.xaml.cpp
// Implementation of the StartPage class
//

#include "pch.h"
#include "StartPage.xaml.h"
#include "CodeEditor.xaml.h"
#include "NewFileDialog.xaml.h"
#include "Editor_Tools.h"

using namespace Just_Editor;

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace concurrency;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238
Platform::String^ TokenString;

StartPage::StartPage()
{
	InitializeComponent();
	LoadRecentList();
}

void StartPage::LoadRecentList()
{
	//Get Recent File List
	create_task(Editor_Tools::ReadFileInAppFolderAsync("User_Files", "RecentList")).then([this](task<String^> thisTask) 
	{
		try
		{
			TokenString = thisTask.get();
			if (TokenString == nullptr)
				return;
		}
		catch (Exception^ WTF)
		{
			//WTF->Message;
			return;
		}
		//thisItem->FileName = recentFileList->Name;
		auto thisPath = (wchar_t*)TokenString->Data();
		int QMIndex = (int)Editor_Tools::FindStr(thisPath, L"?");
		while (QMIndex != -1)
		{
			auto thisItem = ref new RecentListItem;

			String^ thisToken = ref new Platform::String(Editor_Tools::SubStr(thisPath, 0, QMIndex));

			thisItem->Token = thisToken;

			create_task(Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->GetFileAsync(thisToken)).then(
				[thisItem, this]
			(task<Windows::Storage::StorageFile^>thisTask)
			{
				Windows::Storage::StorageFile^ thisFile;
				try
				{
					thisFile = thisTask.get();
				}
				catch (Exception^)
				{
					TokenString = Editor_Tools::ReplacePStr(TokenString, thisItem->Token + L"?", L"");
					Editor_Tools::WriteInAppFile("User_Files", "RecentList", TokenString);
					return;
				}
				thisItem->FilePath = thisFile->Path;
				thisItem->FileName = thisFile->Name;
				thisItem->Foreground = thisData->TopBar_BackgroundBrush;

				((Windows::UI::Xaml::Controls::Button^)((Windows::UI::Xaml::Controls::Grid^)thisItem->Content)->Children->GetAt(1))->Click += ref new Windows::UI::Xaml::RoutedEventHandler(
					[thisItem, this](Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args)
				{
					TokenString = Editor_Tools::ReplacePStr(TokenString, thisItem->Token + L"?", L"");
					Editor_Tools::WriteInAppFile("User_Files", "RecentList", TokenString);

					int ItemIndex = GetRecentItemIndex(thisItem, RecentListPanel);
					RecentListPanel->Children->RemoveAt(ItemIndex);
					//Editor_Tools::FindAllStr(thisString->Data(), L"?")->GetAt(ItemIndex);
				}
				);

				((Windows::UI::Xaml::Controls::Grid^)thisItem->Content)->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler([this, thisItem](Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ args)
				{
					thisItem->Background = ref new Windows::UI::Xaml::Media::SolidColorBrush;
					concurrency::create_task(Windows::Storage::StorageFile::GetFileFromPathAsync(thisItem->FilePath)).then([this](task<Windows::Storage::StorageFile^> thisTask)
					{
						try
						{
							Windows::Storage::StorageFile^ thisFile = thisTask.get();
							this->Frame->Navigate(CodeEditor::typeid, thisFile, ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo);
						}
						catch (Exception^ WTF)
						{
							//Load From Path Error
							Editor_Tools::ShowMessageBox("Oops", "Couldn't open this file!");
						}
					});
				});

				RecentListPanel->Children->Append(thisItem);
			}, task_continuation_context::use_current());

			thisPath = Editor_Tools::SubStr(thisPath, QMIndex + 1);

			QMIndex = (int)Editor_Tools::FindStr(thisPath, L"?");
		}
	}, task_continuation_context::use_current());

	//auto thisFile = ref new Windows::Storage::StorageFile;
	//thisFile->GetFileFromPathAsync();
}

int StartPage::GetRecentItemIndex(RecentListItem^ sender, Windows::UI::Xaml::Controls::Panel^ thisWindowPanel)
{
	int  t = thisWindowPanel->Children->Size;
	while (--t >= 0)
	{
		if (sender == (RecentListItem^)thisWindowPanel->Children->GetAt(t))
		{
			return t;
		}
	}
	return -1;
}

void Just_Editor::StartPage::Page_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	if (e->NewSize.Width < 600)
	{
		MainColumn1->Width = GridLength(1, GridUnitType::Star);
		MainColumn2->Width = GridLength(0);
	}
	else
	{
		MainColumn1->Width = GridLength(250);
		MainColumn2->Width = GridLength(1, GridUnitType::Star);
	}
}


void Just_Editor::StartPage::NewOptionView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	switch (NewOptionView->SelectedIndex)
	{
	case 0:
		Frame->Navigate(CodeEditor::typeid, "Untitled", ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo);
		break;
	case 1:
		auto ThisFileDialog = ref new NewFileDialog;
		ThisFileDialog->Background = thisData->ToolBar_BackgroundBrush;
		ThisFileDialog->Closed +=
			ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::ContentDialog^, Windows::UI::Xaml::Controls::ContentDialogClosedEventArgs^>(
				[this, ThisFileDialog](Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogClosedEventArgs^ args)
		{
			delete[] sender;
		});
		((Windows::UI::Xaml::Controls::Button^)((Windows::UI::Xaml::Controls::Grid^)((Windows::UI::Xaml::Controls::Grid^)ThisFileDialog->Content)->Children->GetAt(3))->Children->GetAt(1))->Click +=
			ref new RoutedEventHandler([this, ThisFileDialog](Object^ sender, RoutedEventArgs^ e) 
		{
			if (ThisFileDialog->FileName != nullptr && ThisFileDialog->FileName != "")
			{
				this->Frame->Navigate(CodeEditor::typeid, ThisFileDialog->FileName, ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo);
			}
		});
			
		create_task(ThisFileDialog->ShowAsync());
		
		break;
	}
	NewOptionView->SelectedItem = nullptr;
}


void Just_Editor::StartPage::OpenOptionView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	switch (OpenOptionView->SelectedIndex)
	{
	case 0:
		auto filePicker = ref new Windows::Storage::Pickers::FileOpenPicker;
		filePicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::Thumbnail;
		filePicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::DocumentsLibrary;

		filePicker->FileTypeFilter->Append(".txt");
		filePicker->FileTypeFilter->Append(".h");
		filePicker->FileTypeFilter->Append(".c");
		filePicker->FileTypeFilter->Append(".cpp");
		filePicker->FileTypeFilter->Append(".cs");
		filePicker->FileTypeFilter->Append(".js");
		filePicker->FileTypeFilter->Append(".php");
		filePicker->FileTypeFilter->Append(".css");
		filePicker->FileTypeFilter->Append(".html");
		filePicker->FileTypeFilter->Append(".xml");
		filePicker->FileTypeFilter->Append(".xaml");
		filePicker->FileTypeFilter->Append(".sln");

		create_task(filePicker->PickSingleFileAsync()).then([this] (task<Windows::Storage::StorageFile^> thisTask)
		{
			try
			{
				Windows::Storage::StorageFile^ thisFile = thisTask.get();
				if (thisFile != nullptr)
				{
					Editor_Tools::AddToRecentFile(thisFile);
					this->Frame->Navigate(CodeEditor::typeid, thisFile, ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo);
				}
			}
			catch (Exception^ WTF)
			{
				//Opps!
				Editor_Tools::ShowMessageBox("Tips", "Read failed!\n" + WTF->Message);
			}
		}, task_continuation_context::use_current());

		break;
	}

	OpenOptionView->SelectedItem = nullptr;
}
