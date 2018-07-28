//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "StartPage.xaml.h"
#include "CodeEditor.xaml.h"
#include "Editor_Tools.h"
#include "RenameDialog.xaml.h"
#include "DuronWindowItemxaml.xaml.h"

using namespace Just_Editor;

using namespace Platform;
using namespace Windows::UI::Xaml::Controls;
using namespace concurrency;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409
//int lastWindowIndex;

MainPage::MainPage()
{
	InitializeComponent();
	InitializePage();

	NewWindowItem("StartPage", "?S");
}

void MainPage::InitializePage()
{
	//Editor_Tools::DeleteFileInAppAsync("User_Files", "RecentList");
	auto titleBar = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->TitleBar;
	titleBar->ForegroundColor = Windows::UI::Colors::WhiteSmoke;
	titleBar->ButtonForegroundColor = titleBar->ForegroundColor;
	titleBar->ButtonHoverBackgroundColor = titleBar->ForegroundColor;
	titleBar->BackgroundColor = Windows::UI::Colors::Gray;
	titleBar->ButtonBackgroundColor = titleBar->BackgroundColor;
	if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
	{
		auto statusBar = Windows::UI::ViewManagement::StatusBar::GetForCurrentView();

		Windows::UI::Color StatusBarBackColor;
		StatusBarBackColor.R = 120;
		StatusBarBackColor.G = 120;
		StatusBarBackColor.B = 120;
		statusBar->BackgroundColor = StatusBarBackColor;

		statusBar->BackgroundOpacity = 1;
		statusBar->ForegroundColor = Windows::UI::Colors::LightGray;
		statusBar->ShowAsync();
	}
}

void MainPage::NewWindowItem(Platform::String^ File_Name, Platform::String^ File_Path = "" , bool AutoSelect, Platform::Object^ Frame_Content, Windows::Storage::StorageFile^ Item_File, bool isChanged)
{
	auto thisItem = ref new DuronWindowItemxaml;

	thisItem->FileName = File_Name;
	thisItem->FilePath = File_Path;
	if (Frame_Content != nullptr)
	{
		thisItem->FrameContent = Frame_Content;
		if (isChanged)
		{
			thisItem->SetChanged(true);
		}
	}
	if (Item_File != nullptr)
		thisItem->ItemFile = Item_File;

	WindowPanel->Children->Append(thisItem);


	thisItem->Tapped +=
		ref new Windows::UI::Xaml::Input::TappedEventHandler(this,
		&MainPage::WindowItem_Tapped);

	if (File_Name != "?S")
	{
		thisItem->RightTapped +=
			ref new Windows::UI::Xaml::Input::RightTappedEventHandler(this,
				&MainPage::WindowItem_RightTapped);
	}

	((Button^)((Grid^)thisItem->Content)->Children->GetAt(1))->Click += 
		ref new Windows::UI::Xaml::RoutedEventHandler(this,
		&MainPage::WindowItemCloseButton_Click);

	if(AutoSelect)
		WindowSelectAt(WindowPanel->Children->Size - 1);
}

void MainPage::WindowUnSelectAt(int Item_Index)
{
	DuronWindowItemxaml^ thisItem = (DuronWindowItemxaml^)WindowPanel->Children->GetAt(Item_Index);
	if (thisItem->isSelected)
	{
		//((DuronWindowItemxaml^)WindowPanel->Children->GetAt(Item_Index))->Background = DefaultBrush;
		thisItem->Unselect();
	}
}

void MainPage::WindowUnSelectAll()
{
	int i = (int)WindowPanel->Children->Size;
	while (-- i >= 0)
	{
		if (((DuronWindowItemxaml^)WindowPanel->Children->GetAt(i))->isSelected)
		{
			//((DuronWindowItemxaml^)WindowPanel->Children->GetAt(i))->Background = DefaultBrush;
			((DuronWindowItemxaml^)WindowPanel->Children->GetAt(i))->Unselect();
		}
	}
}

void MainPage::WindowSelectAt(int Item_Index)
{
	if (Item_Index < 0)
	{
		if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
		{
			App::Current->Exit();
		}
		else
		{
			exit(Item_Index);
		}
	}


	WindowUnSelectAll();
	//((DuronWindowItemxaml^)WindowPanel->Children->GetAt(Item_Index))->Background = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::White);
	((DuronWindowItemxaml^)WindowPanel->Children->GetAt(Item_Index))->Select();

	auto thisItem = (DuronWindowItemxaml^)WindowPanel->Children->GetAt(Item_Index);

	if (thisItem->FrameContent != nullptr)
	{
		MainFrame->Content = thisItem->FrameContent;
	}
	else if (thisItem->FilePath != "?S")
	{
		MainFrame->Navigate(CodeEditor::typeid);
	}
	else
	{
		MainFrame->Navigate(StartPage::typeid);
	}
	//thisItem->SetChanged(false);
}


void MainPage::RemoveWindowItem(DuronWindowItemxaml^ sender)
{
	auto thisPanel = (Panel^)sender->Parent;
	int Item_Index = GetWindowItemIndex(sender, thisPanel);

	delete sender->ItemFile;

	thisPanel->Children->RemoveAt(Item_Index);
	if (((DuronWindowItemxaml^)sender)->isSelected)
	{
		WindowSelectAt(WindowPanel->Children->Size - 1);
	}
}

int MainPage::GetSelectedItemIndex()
{
	int  t = WindowPanel->Children->Size;
	while (--t >= 0)
	{
		if (((DuronWindowItemxaml^)WindowPanel->Children->GetAt(t))->isSelected)
		{
			return t;
		}
	}
	return -1;
}

int MainPage::GetWindowItemIndex(DuronWindowItemxaml^ sender, Windows::UI::Xaml::Controls::Panel^ thisWindowPanel)
{
	int  t = thisWindowPanel->Children->Size;
	while (--t >= 0)
	{
		if (sender == (DuronWindowItemxaml^)thisWindowPanel->Children->GetAt(t))
		{
			return t;
		}
	}
	return -1;
}

void MainPage::CheckWindowItem()
{
	DuronWindowItemxaml^ thisItem;
	double WidthForWindowPanel = TopStackPanel->ActualWidth - GetHiddenWindow_Button->ActualWidth - AddWindow_Button->ActualWidth;
	if (WindowPanel->ActualWidth >= WidthForWindowPanel && WindowPanel->Children->Size > 1)
	{
		thisItem = (DuronWindowItemxaml^)WindowPanel->Children->GetAt(0);

		auto targetItem = ref new DuronWindowItemxaml;
		targetItem->FileName = thisItem->FileName;
		targetItem->FilePath = thisItem->FilePath;
		targetItem->FrameContent = thisItem->FrameContent;
		targetItem->ItemFile = thisItem->ItemFile;

		targetItem->isChanged = thisItem->isChanged;

		targetItem->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler([targetItem, this](Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ args) 
		{

			HiddenWindowPanel->Children->RemoveAt(GetWindowItemIndex(targetItem, HiddenWindowPanel));

			NewWindowItem(targetItem->FileName, targetItem->FilePath, true, targetItem->FrameContent, targetItem->ItemFile, targetItem->isChanged);

			delete[] sender;
		});

		((Button^)((Grid^)targetItem->Content)->Children->GetAt(1))->Click +=
			ref new Windows::UI::Xaml::RoutedEventHandler(this,
				&MainPage::WindowItemCloseButton_Click);

		HiddenWindowPanel->Children->Append(targetItem);


		Windows::UI::Xaml::UIElement^ removeItem = WindowPanel->Children->GetAt(0);
		WindowPanel->Children->RemoveAt(0);

		delete[] removeItem;
	}

	if (WidthForWindowPanel - WindowPanel->ActualWidth >= HiddenWindowPanel->ActualWidth && HiddenWindowPanel->Children->Size > 0)
	{
		thisItem = (DuronWindowItemxaml^)HiddenWindowPanel->Children->GetAt(HiddenWindowPanel->Children->Size - 1);
		HiddenWindowPanel->Children->RemoveAtEnd();
		NewWindowItem(thisItem->FileName, thisItem->FilePath, false, thisItem->FrameContent, thisItem->ItemFile, thisItem->isChanged);
		thisItem = nullptr;
	}
}

void MainPage::WindowItemCloseButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto thisItem = (DuronWindowItemxaml^)((Grid^)((Button^)sender)->Parent)->Parent;
	if (thisItem->isChanged)
	{
		auto theDialog = Editor_Tools::GetContentDialog("Tips", "Do you want to save it?", true, true);
		theDialog->PrimaryButtonClick += ref new Windows::Foundation::TypedEventHandler<ContentDialog^, ContentDialogButtonClickEventArgs^>([thisItem] (ContentDialog^ sender, ContentDialogButtonClickEventArgs^ args)
		{
			((CodeEditor^)thisItem->FrameContent)->SaveFile();
		});
		theDialog->CloseButtonClick += ref new Windows::Foundation::TypedEventHandler<ContentDialog^, ContentDialogButtonClickEventArgs^>([](ContentDialog^ sender, ContentDialogButtonClickEventArgs^ args)
		{
			sender->CloseButtonText = "";
		});
		theDialog->Closed += ref new Windows::Foundation::TypedEventHandler<ContentDialog^, ContentDialogClosedEventArgs^>([thisItem, this](ContentDialog^ sender, ContentDialogClosedEventArgs^ args)
		{
			if (sender->CloseButtonText != "")
			{
				RemoveWindowItem(thisItem);
			}
			delete sender;
		});
		theDialog->ShowAsync();

	}
	else
	{
		RemoveWindowItem(thisItem);
	}
	delete[] sender;
	thisItem = nullptr;
}

void MainPage::WindowItem_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	WindowSelectAt(GetWindowItemIndex((DuronWindowItemxaml^)sender, WindowPanel));
}

void MainPage::WindowItem_RightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e)
{
	auto thisItem = (DuronWindowItemxaml^)sender;

	auto renameDialog = ref new RenameDialog;
	renameDialog->FileName = thisItem->FileName;
	renameDialog->Closed += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::ContentDialog^, Windows::UI::Xaml::Controls::ContentDialogClosedEventArgs^>([thisItem, renameDialog]
	(ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogClosedEventArgs^ args)
	{
		thisItem->SetFileName(renameDialog->FileName);
	});
	renameDialog->ShowAsync();

}

void Just_Editor::MainPage::MainFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	if (e->Parameter != nullptr)
	{
		if (e->Parameter->ToString() == L"Windows.Storage.StorageFile")
		{
			Windows::Storage::StorageFile^ ItemFile = (Windows::Storage::StorageFile^)e->Parameter;
			NewWindowItem(ItemFile->Name, ItemFile->Path, true, nullptr, ItemFile);
		}
		else
		{
			NewWindowItem((Platform::String^)e->Parameter);
		}
	}

	auto thisItem = (DuronWindowItemxaml^)WindowPanel->Children->GetAt(GetSelectedItemIndex());
	if (thisItem->ItemFile != nullptr && thisItem->FrameContent == nullptr)
	{
		create_task(Editor_Tools::ReadFileAsync(thisItem->ItemFile)).then([this, thisItem](task<String^> thisTask)
		{
			try
			{
				String^ thisText = thisTask.get();
				thisItem->OriginalText = thisText;

				((RichEditBox^)((ScrollViewer^)((Panel^)((Page^)MainFrame->Content)->Content)->Children->GetAt(1))->Content)->Document->Selection->Text += thisText;

				((CodeEditor^)MainFrame->Content)->thisWindowItem = thisItem;

			}
			catch (Exception^ WTF)
			{
				//Read Fail
				Editor_Tools::ShowMessageBox("Tips", "Read failed!\n" + WTF->Message);
			}
		}, task_continuation_context::use_current());
	}
	else if (thisItem->FilePath != "?S" && ((CodeEditor^)MainFrame->Content)->thisWindowItem == nullptr)
	{
		((CodeEditor^)MainFrame->Content)->thisWindowItem = thisItem;
		/*		((RichEditBox^)((ScrollViewer^)((Panel^)((Page^)MainFrame->Content)->Content)->Children->GetAt(1))->Content)->TextChanged +=
			ref new Windows::UI::Xaml::RoutedEventHandler([thisItem](Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args)
		{
			String^ thisText = "";
			((RichEditBox^)sender)->Document->GetText(Windows::UI::Text::TextGetOptions::None, &thisText);
			if(thisText != thisItem->OriginalText)
				thisItem->SetChanged(true);
			else
				thisItem->SetChanged(false);
		});*/
	}
	thisItem->FrameContent = MainFrame->Content;
}


void Just_Editor::MainPage::AddWindow_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	NewWindowItem("Untitled " + WindowPanel->Children->Size);
}


void Just_Editor::MainPage::SettingsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	SettingsSplit->IsPaneOpen = !SettingsSplit->IsPaneOpen;
}


void Just_Editor::MainPage::HomeButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	NewWindowItem("StartPage", "?S");
}


void Just_Editor::MainPage::GetHiddenWindow_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	HiddenWindowPanel->Height = HiddenWindowPanel->Height ? 0 : Height;
}



void Just_Editor::MainPage::WindowPanel_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	CheckWindowItem();
}
