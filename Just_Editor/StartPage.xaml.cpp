//
// StartPage.xaml.cpp
// Implementation of the StartPage class
//

#include "pch.h"
#include "StartPage.xaml.h"
#include "CodeEditor.xaml.h"
#include "NewFileDialog.xaml.h"

using namespace Just_Editor;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Concurrency;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

StartPage::StartPage()
{
	InitializeComponent();
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
		filePicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::PicturesLibrary;

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
			catch (Exception^)
			{
				//Opps!
			}
		}, task_continuation_context::use_current());

		break;
	}

	OpenOptionView->SelectedItem = nullptr;
}
