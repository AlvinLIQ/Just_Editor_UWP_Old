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

StartPage::StartPage()
{
	InitializeComponent();
	LoadRecentList();
}

void StartPage::LoadRecentList()
{
	/*
	Platform::Object^ SItem = Editor_Tools::ReadSetting("Recent", "FileList");
	if (SItem != nullptr)
	{
		String^ recentFileList = (String^)SItem;
		//thisItem->FileName = recentFileList->Name;
		auto thisPath = (wchar_t*)recentFileList->Data();
		int QMIndex = QMIndex = (int)Editor_Tools::FindStr(thisPath, L"?");
		while(QMIndex != -1)
		{
			auto thisItem = ref new RecentListItem;
			thisItem->FilePath = ref new Platform::String(Editor_Tools::SubStr(thisPath, 0, QMIndex));
			thisItem->FileName = Editor_Tools::GetFileNameFromPath(thisItem->FilePath);
			RecentListPanel->Children->Append(thisItem);

			thisPath = Editor_Tools::SubStr(thisPath, QMIndex + 1);

			QMIndex = (int)Editor_Tools::FindStr(thisPath, L"?");
		}
	}*/

	create_task(Editor_Tools::ReadFileInAppFolderAsync("User_Files", "RecentList")).then([this](task<String^> thisTask) 
	{
		String^ thisString;
		try
		{
			thisString = thisTask.get();
			if (thisString == nullptr)
				return;
		}
		catch (Exception^ WTF)
		{
			//WTF->Message;
		}
		//thisItem->FileName = recentFileList->Name;
		auto thisPath = (wchar_t*)thisString->Data();
		int QMIndex = QMIndex = (int)Editor_Tools::FindStr(thisPath, L"?");
		while (QMIndex != -1)
		{
			auto thisItem = ref new RecentListItem;
			thisItem->FilePath = ref new Platform::String(Editor_Tools::SubStr(thisPath, 0, QMIndex));
			thisItem->FileName = Editor_Tools::GetFileNameFromPath(thisItem->FilePath);

			((Windows::UI::Xaml::Controls::Button^)((Windows::UI::Xaml::Controls::Grid^)thisItem->Content)->Children->GetAt(1))->Click += ref new Windows::UI::Xaml::RoutedEventHandler(
				[thisItem, thisString, this](Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args) 
			{
				//Editor_Tools::WriteInAppFile("User_Files", "RecentList", Editor_Tools::ReplacePStr(thisString->Data(), (thisItem->FilePath + L"?")->Data(), L""));
				
				int ItemIndex = GetRecentItemIndex(thisItem, RecentListPanel);
				RecentListPanel->Children->RemoveAt(ItemIndex);
				//Editor_Tools::FindAllStr(thisString->Data(), L"?")->GetAt(ItemIndex);
			}
			);

			RecentListPanel->Children->Append(thisItem);

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
		Frame->Navigate(CodeEditor::typeid, "Untitled");
		break;
	case 1:
		auto ThisFileDialog = ref new NewFileDialog;
		ThisFileDialog->Closed +=
			ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::ContentDialog^, Windows::UI::Xaml::Controls::ContentDialogClosedEventArgs^>(
				[this, ThisFileDialog](Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogClosedEventArgs^ args)
		{
			if (ThisFileDialog->FileName != nullptr && ThisFileDialog->FileName != "")
			{
				this->Frame->Navigate(CodeEditor::typeid, ThisFileDialog->FileName);
			}
		});
			
		ThisFileDialog->ShowAsync();
		
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

		create_task(filePicker->PickSingleFileAsync()).then([this] (task<Windows::Storage::StorageFile^> thisTask)
		{
			try
			{
				Windows::Storage::StorageFile^ thisFile = thisTask.get();
				if(thisFile != nullptr)
					this->Frame->Navigate(CodeEditor::typeid, thisFile);
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
