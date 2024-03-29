﻿//
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
#include "DuronSmartDetect.xaml.h"
#include "CaesarPanel.xaml.h"

using namespace Just_Editor;

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace concurrency;

int OldIndex = -1;
Windows::Foundation::TimeSpan ts;

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
	thisData = ref new Editor_Data();
	DarkSwitch->IsOn = thisData->isDark;
	DetectSwitch->IsOn = thisData->isSmartDetectEnabled;
	HighlightSwitch->IsOn = thisData->isHighlightEnabled;
	LineNumSwitch->IsOn = thisData->isLineNumEnabled;
	auto thisToggledEvent = ref new RoutedEventHandler(this, &MainPage::Switch_Toggled);
	DarkSwitch->Toggled += thisToggledEvent;
	DetectSwitch->Toggled += thisToggledEvent;
	HighlightSwitch->Toggled += thisToggledEvent;
	LineNumSwitch->Toggled += thisToggledEvent;

	auto titleBar = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->TitleBar;
	titleBar->ForegroundColor = Windows::UI::Colors::WhiteSmoke;
	titleBar->ButtonForegroundColor = Windows::UI::Colors::LightGray;
	titleBar->ButtonHoverBackgroundColor = Windows::UI::Colors::DarkGray;
	titleBar->BackgroundColor = Windows::UI::Colors::Gray;
	Windows::UI::Color TopButton_Color;
	TopButton_Color.A = 0;
	TopButton_Color.R = 0;
	TopButton_Color.G = 0;
	TopButton_Color.B = 0;
	titleBar->ButtonBackgroundColor = TopButton_Color;
	auto CoreTitleBar = Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->TitleBar;
	CoreTitleBar->ExtendViewIntoTitleBar = true;
	Windows::UI::Xaml::Window::Current->SetTitleBar(TitleBar_Block);

	/*
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
	else*/
	if (!Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
		TopBar_Grid->Margin = Windows::UI::Xaml::Thickness(0, 0, 182, 0);
	else
	{
		TopBar_Grid->Height = 40;
		Windows::UI::ViewManagement::StatusBar::GetForCurrentView()->HideAsync();
	}

	ts.Duration = 5;

	create_task(Windows::Storage::ApplicationData::Current->LocalFolder->GetFilesAsync()).then([this] (task<Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile ^>^> thisTask) 
	{
		auto Files = thisTask.get();
		create_task(Editor_Tools::ReadFileInAppFolderAsync("User_Files", "Background")).then([this, Files](String ^fileName)
		{
			Windows::Storage::StorageFile^ thisFile;
			int cSize = Files->Size;
			while (--cSize >= 0)
			{
				thisFile = Files->GetAt(cSize);
				if (thisFile->Name == fileName)
				{
					Background_Update(thisFile);
					return;
				}
			}
			auto bgImage = ref new Media::Imaging::BitmapImage();
			bgImage->UriSource = ref new Windows::Foundation::Uri("ms-appx:///Assets/Background.png");
			BackgroundIMG->Source = bgImage;
		}, task_continuation_context::use_current());
	}, task_continuation_context::use_current());
}

void MainPage::NewWindowItem(Platform::String ^File_Name, Platform::String ^File_Path = "" , bool AutoSelect, Platform::Object ^Frame_Content, Windows::Storage::StorageFile ^Item_File, bool isChanged)
{
	auto thisItem = ref new DuronWindowItemxaml;
	thisItem->FileName = File_Name;
	thisItem->FilePath = File_Path;
	if (Frame_Content != nullptr)
	{
		thisItem->FrameContent = Frame_Content;
		if (isChanged)
			thisItem->SetChanged(true);
	}
	if (Item_File != nullptr)
		thisItem->ItemFile = Item_File;

	thisItem->thisData = thisData;

	thisItem->SetDisplayName("");
	thisItem->Width = 0;
	WindowPanel->Children->Append(thisItem);

	//Show
	auto thisTimer = ref new DispatcherTimer;
	thisTimer->Interval = ts;
	thisTimer->Tick += ref new Windows::Foundation::EventHandler<Object^>([this, thisTimer, thisItem, AutoSelect, File_Name, File_Path](Object ^sender, Object ^e)
	{
		if (120 > thisItem->ActualWidth)
			thisItem->Width += 30;
		else
		{
			thisTimer->Stop();
			delete[] thisTimer;

			
			thisItem->SetDisplayName(File_Name);
			if (AutoSelect)
				WindowSelectAt(WindowPanel->Children->Size - 1);

			CheckWindowItem();
			thisItem->Tapped +=
				ref new Windows::UI::Xaml::Input::TappedEventHandler(this,
					&MainPage::WindowItem_Tapped);

			if (File_Path != "?S")
			{
				thisItem->RightTapped +=
					ref new Windows::UI::Xaml::Input::RightTappedEventHandler(this,
						&MainPage::WindowItem_RightTapped);

				if (thisItem->FrameContent == nullptr)
				{
					MainFrame->Navigate(CodeEditor::typeid, nullptr, ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo);
					thisItem->FrameContent = MainFrame->Content;
				}
				CodeEditor ^thisEditor = (CodeEditor^)thisItem->FrameContent;
				thisEditor->thisWindowItem = thisItem;

				if (thisItem->ItemFile != nullptr)
				{
					thisEditor->isDetecting = true;
					Editor_Tools::AddToRecentFile(thisItem->ItemFile);

					create_task(Editor_Tools::ReadFileAsync(thisItem->ItemFile)).then([this, thisItem, thisEditor](task<String^> thisTask)
					{
						RichEditBox ^EditBox = thisEditor->GetEditBox();
						EditBox->Document->EndUndoGroup();
						EditBox->Document->UndoLimit = 0;
						try
						{
							String ^thisText = thisTask.get();
							//((RichEditBox^)((Grid^)((ScrollViewer^)((Panel^)((Page^)MainFrame->Content)->Content)->Children->GetAt(1))->Content)->Children->GetAt(0))->Document->Selection->Text += thisText;
							EditBox->Document->SetText(Windows::UI::Text::TextSetOptions::None, thisText);
							//EditBox->Document->Selection->Text += thisText;
							//EditBox->Document->Selection->EndPosition = 0;
						}
						catch (Exception ^WTF)
						{
							//Read Fail
							Editor_Tools::ShowMessageBox("Tips", "Read failed!\n" + WTF->Message);
						}
						thisEditor->AutoDetect(0, Windows::UI::Text::TextConstants::MaxUnitCount, true);
					});
				}
			}
			thisItem->PointerPressed +=
				ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &MainPage::WindowItem_Pressed);

			thisItem->PointerReleased +=
				ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &MainPage::WindowItem_Released);

			((Button^)((Grid^)thisItem->Content)->Children->GetAt(1))->Click +=
				ref new Windows::UI::Xaml::RoutedEventHandler(this,
					&MainPage::WindowItemCloseButton_Click);

			
			
		}
	});
	thisTimer->Start();

}

void MainPage::WindowUnSelectAt(int Item_Index)
{
	DuronWindowItemxaml ^thisItem = (DuronWindowItemxaml^)WindowPanel->Children->GetAt(Item_Index);
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
		if (HiddenWindowPanel->Children->Size)
		{
			CheckWindowItem();
		}
		else
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
		return;
	}


	WindowUnSelectAll();
	//((DuronWindowItemxaml^)WindowPanel->Children->GetAt(Item_Index))->Background = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::White);

	auto thisItem = (DuronWindowItemxaml^)WindowPanel->Children->GetAt(Item_Index);
	thisItem->Select();

	if (thisItem->FrameContent != nullptr)
	{
		MainFrame->Content = thisItem->FrameContent;
	}
	else
	{		
		if (thisItem->FilePath != "?S")
		{
			MainFrame->Navigate(CodeEditor::typeid, nullptr, ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo);
			((CodeEditor^)MainFrame->Content)->thisWindowItem = thisItem;
		}
		else
			MainFrame->Navigate(StartPage::typeid, nullptr, ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo);

		thisItem->FrameContent = MainFrame->Content;
	}
	//thisItem->SetChanged(false);
}

void MainPage::RemoveWindowItem(DuronWindowItemxaml ^sender)
{
	delete sender->ItemFile;
	sender->SetDisplayName("");
	//Hide && Remove
	auto thisTimer = ref new DispatcherTimer;
	thisTimer->Interval = ts;
	thisTimer->Tick += ref new Windows::Foundation::EventHandler<Object^>([this, thisTimer, thisItem = sender](Object ^sender, Object ^e)
	{
		if (30 <= thisItem->ActualWidth)
			thisItem->Width -= 30;
		else
		{
			thisTimer->Stop();
			delete[] thisTimer;

			auto thisPanel = (Panel^)thisItem->Parent;
			int Item_Index = GetWindowItemIndex(thisItem, thisPanel);
			thisPanel->Children->RemoveAt(Item_Index);

			if (thisItem->isSelected)
			{
				WindowSelectAt(WindowPanel->Children->Size - 1);
			}
			CheckWindowItem();

		}
	});
	thisTimer->Start();
	
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

int MainPage::GetWindowItemIndex(DuronWindowItemxaml ^sender, Windows::UI::Xaml::Controls::Panel ^thisWindowPanel)
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
	DuronWindowItemxaml ^thisItem;
	double WidthForWindowPanel = TopStackPanel->ActualWidth - GetHiddenWindow_Button->ActualWidth - AddWindow_Button->ActualWidth - (TopBar_Grid->Margin.Right / 3);
	if (WindowPanel->ActualWidth >= WidthForWindowPanel && WindowPanel->Children->Size > 1)
	{
		unsigned int itemIndex = 0;
		thisItem = (DuronWindowItemxaml^)WindowPanel->Children->GetAt(itemIndex);
		if(thisItem->isSelected)
			thisItem = (DuronWindowItemxaml^)WindowPanel->Children->GetAt(++itemIndex);

		auto targetItem = ref new DuronWindowItemxaml;
		targetItem->FileName = thisItem->FileName;
		targetItem->FilePath = thisItem->FilePath;
		targetItem->FrameContent = thisItem->FrameContent;
		targetItem->ItemFile = thisItem->ItemFile;
		targetItem->Width = thisItem->Width;
		targetItem->thisData = thisItem->thisData;

		targetItem->isChanged = thisItem->isChanged;

		targetItem->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler([targetItem, this](Platform::Object ^sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^args) 
		{

			HiddenWindowPanel->Children->RemoveAt(GetWindowItemIndex(targetItem, HiddenWindowPanel));

			NewWindowItem(targetItem->FileName, targetItem->FilePath, true, targetItem->FrameContent, targetItem->ItemFile, targetItem->isChanged);

			delete[] sender;
		});

		targetItem->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &MainPage::WindowItem_Pressed);
		targetItem->PointerReleased += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &MainPage::WindowItem_Released);

		((Button^)((Grid^)targetItem->Content)->Children->GetAt(1))->Click +=
			ref new Windows::UI::Xaml::RoutedEventHandler(this,
				&MainPage::WindowItemCloseButton_Click);

		HiddenWindowPanel->Children->Append(targetItem);


		Windows::UI::Xaml::UIElement ^removeItem = WindowPanel->Children->GetAt(itemIndex);
		WindowPanel->Children->RemoveAt(itemIndex);

		delete[] removeItem;
	}

	if (WidthForWindowPanel - WindowPanel->ActualWidth >= HiddenWindowPanel->ActualWidth && HiddenWindowPanel->Children->Size > 0)
	{
		thisItem = (DuronWindowItemxaml^)HiddenWindowPanel->Children->GetAt(HiddenWindowPanel->Children->Size - 1);
		HiddenWindowPanel->Children->RemoveAtEnd();
		NewWindowItem(thisItem->FileName, thisItem->FilePath, !WindowPanel->Children->Size, thisItem->FrameContent, thisItem->ItemFile, thisItem->isChanged);
		thisItem = nullptr;
	}
	HiddenMenuTrans->X = WindowPanel->Children->Size * 120;
}

void MainPage::WindowItemCloseButton_Click(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	auto thisItem = (DuronWindowItemxaml^)((Grid^)((Button^)sender)->Parent)->Parent;
	if (thisItem->isChanged)
	{
		auto theDialog = Editor_Tools::GetContentDialog("Tips", "Do you want to save it?", true, true);
		theDialog->Background = thisData->ToolBar_BackgroundBrush;
		theDialog->Foreground = thisData->Editor_ForegroundBrush;
		theDialog->RequestedTheme = thisData->isDark ? Windows::UI::Xaml::ElementTheme::Light : Windows::UI::Xaml::ElementTheme::Dark;

		theDialog->PrimaryButtonClick += ref new Windows::Foundation::TypedEventHandler<ContentDialog^, ContentDialogButtonClickEventArgs^>([thisItem, this] (ContentDialog ^sender, ContentDialogButtonClickEventArgs ^args)
		{
			((CodeEditor^)thisItem->FrameContent)->SaveFile();
			RemoveWindowItem(thisItem);
		});
		theDialog->SecondaryButtonClick += ref new Windows::Foundation::TypedEventHandler<ContentDialog^, ContentDialogButtonClickEventArgs^>([thisItem, this](ContentDialog ^sender, ContentDialogButtonClickEventArgs ^args)
		{
			RemoveWindowItem(thisItem);
		});
		theDialog->Closed += ref new Windows::Foundation::TypedEventHandler<ContentDialog^, ContentDialogClosedEventArgs^>([](ContentDialog ^sender, ContentDialogClosedEventArgs ^args)
		{
			delete[] sender;
		});
		create_task(theDialog->ShowAsync());

	}
	else
	{
		RemoveWindowItem(thisItem);
	}
	delete[] sender;
	thisItem = nullptr;
}

void MainPage::WindowItem_Tapped(Platform::Object ^sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^e)
{
	WindowSelectAt(GetWindowItemIndex((DuronWindowItemxaml^)sender, WindowPanel));
}

void MainPage::WindowItem_RightTapped(Platform::Object ^sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs ^e)
{
	auto thisItem = (DuronWindowItemxaml^)sender;

	auto renameDialog = ref new RenameDialog;
	renameDialog->Background = thisData->ToolBar_BackgroundBrush;
	renameDialog->RequestedTheme = thisData->isDark ? Windows::UI::Xaml::ElementTheme::Light : Windows::UI::Xaml::ElementTheme::Dark;
	renameDialog->FileName = thisItem->FileName;
	renameDialog->Closed += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::ContentDialog^, Windows::UI::Xaml::Controls::ContentDialogClosedEventArgs^>([thisItem, renameDialog]
	(ContentDialog ^sender, Windows::UI::Xaml::Controls::ContentDialogClosedEventArgs ^args)
	{
		thisItem->SetFileName(renameDialog->FileName);
	});
	renameDialog->ShowAsync();

}

void MainPage::WindowItem_Pressed(Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e)
{
	OldIndex = GetWindowItemIndex((DuronWindowItemxaml^)sender, (Panel^)((DuronWindowItemxaml^)sender)->Parent);
}

void MainPage::WindowItem_Released(Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e)
{
	if (OldIndex != -1)
	{
		int NewIndex = GetWindowItemIndex((DuronWindowItemxaml^)sender, (Panel^)((DuronWindowItemxaml^)sender)->Parent);
		if (NewIndex != -1)
			((Panel^)((DuronWindowItemxaml^)sender)->Parent)->Children->Move(OldIndex, NewIndex);
	}
	OldIndex = -1;
}

void MainPage::OpenFromStorageFile(Windows::Storage::StorageFile ^thisFile, bool AutoSelect)
{
	NewWindowItem(thisFile->Name, thisFile->Path, AutoSelect, nullptr, thisFile, false);
}

void Just_Editor::MainPage::MainFrame_Navigated(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationEventArgs ^e)
{
	if (e->Parameter != nullptr)
	{
		if (e->Parameter->ToString() == L"Windows.Storage.StorageFile")
		{
			Windows::Storage::StorageFile ^ItemFile = (Windows::Storage::StorageFile^)e->Parameter;
			NewWindowItem(ItemFile->Name, ItemFile->Path, true, MainFrame->Content, ItemFile);
		}
		else
		{
			NewWindowItem((Platform::String^)e->Parameter);
		}
	}

	
	//auto thisItem = (DuronWindowItemxaml^)WindowPanel->Children->GetAt(GetSelectedItemIndex());
	
	if (MainFrame->Content->ToString() == "Just_Editor.StartPage")
		((StartPage^)MainFrame->Content)->thisData = thisData;
	else
	{
		((CodeEditor^)MainFrame->Content)->thisData = thisData;
		((CodeEditor^)MainFrame->Content)->CheckLineNums();
	}
}

void Just_Editor::MainPage::AddWindow_Button_Click(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	NewWindowItem("Untitled " + WindowPanel->Children->Size);
}

void Just_Editor::MainPage::SettingsButton_Click(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	SettingsSplit->IsPaneOpen = !SettingsSplit->IsPaneOpen;
}

void Just_Editor::MainPage::HomeButton_Click(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	NewWindowItem("StartPage", "?S");
}

void Just_Editor::MainPage::GetHiddenWindow_Button_Click(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	ChangeHiddenPanelExpand();
}

void Just_Editor::MainPage::WindowPanel_SizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e)
{
	CheckWindowItem();
}

void Just_Editor::MainPage::Switch_Toggled(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	auto thisSwitch = (ToggleSwitch^)e->OriginalSource;
	Editor_Tools::WriteSetting(thisSwitch->Name, thisSwitch->Name, thisSwitch->IsOn ? L"1" : L"0");
	if (thisSwitch != DarkSwitch && thisSwitch != LineNumSwitch)
	{
		return;
	}


	//once click, play a year!
	thisData = ref new Editor_Data;
	this->Bindings->Update();

	DuronWindowItemxaml ^thisItem;
	int ItemsNum;
	Panel ^thisWindowPanel;
	for (int i = 0; i < 2; i++)
	{
		thisWindowPanel = i ? WindowPanel : HiddenWindowPanel;
		ItemsNum = thisWindowPanel->Children->Size;
		while (--ItemsNum >= 0)
		{
			thisItem = (DuronWindowItemxaml^)thisWindowPanel->Children->GetAt(ItemsNum);
			thisItem->thisData = thisData;
			thisItem->UpdateBindings();
			if (thisItem->isSelected)
				thisItem->Background = thisData->ToolBar_BackgroundBrush;

			if (thisItem->FrameContent != nullptr)
			{
				if (thisItem->FrameContent->ToString() == "Just_Editor.CodeEditor")
				{
					((CodeEditor^)thisItem->FrameContent)->thisData = thisData;
					((CodeEditor^)thisItem->FrameContent)->LineNum = 0;
					((CodeEditor^)thisItem->FrameContent)->LineNums_Panel->Content = "";
					((CodeEditor^)thisItem->FrameContent)->UpdateBindings();

					auto thisMainPanel = (Panel^)((CodeEditor^)thisItem->FrameContent)->Content;
					((DuronSmartDetect^)((Grid^)thisMainPanel->Children->GetAt(1))->Children->GetAt(1))->thisData = thisData;
					((DuronSmartDetect^)((Grid^)thisMainPanel->Children->GetAt(1))->Children->GetAt(1))->UpdateBindings();
					if (thisMainPanel->Children->Size == 5)
					{
						((CaesarPanel^)thisMainPanel->Children->GetAt(4))->thisData = thisData;
						((CaesarPanel^)thisMainPanel->Children->GetAt(4))->UpdateBindings();
					}
				}
				else
				{
					((StartPage^)thisItem->FrameContent)->thisData = thisData;
					((StartPage^)thisItem->FrameContent)->UpdateBindings();
				}
			}
		}
	}
}


void Just_Editor::MainPage::ChangeHiddenPanelExpand()
{
	if (HiddenScrollViewer->ActualHeight && HiddenScrollViewer->ActualHeight < 120)
	{
		return;
	}

	int icNum;
	if (HiddenScrollViewer->ActualHeight)
	{
		GetHiddenWindow_Button->Content = L"\uE011";
		icNum = -40;
	}
	else
	{
		GetHiddenWindow_Button->Content = L"\uE010";
		icNum = 40;
	}
	//Show || Hide
	auto thisTimer = ref new DispatcherTimer;
	thisTimer->Tick += ref new Windows::Foundation::EventHandler<Object^>([thisTimer, thisItem = HiddenScrollViewer, icNum](Object ^sender, Object ^e)
	{
		int NextHeight = (int)thisItem->ActualHeight + icNum;
		if (NextHeight >= 0 && NextHeight <= 120)
			thisItem->Height = NextHeight;
		else
		{
			thisTimer->Stop();
			delete[] thisTimer;
		}
	});
	thisTimer->Start();
}

void Just_Editor::MainPage::Page_Tapped(Platform::Object ^sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^e)
{
	if (HiddenScrollViewer->ActualHeight)
	{
		ChangeHiddenPanelExpand();
	}
}


void Just_Editor::MainPage::BGChoose_Button_Click(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	auto filePicker = ref new Windows::Storage::Pickers::FileOpenPicker;
	filePicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::Thumbnail;
	filePicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::PicturesLibrary;
	filePicker->FileTypeFilter->Append(".jpg");
	filePicker->FileTypeFilter->Append(".bmp");
	filePicker->FileTypeFilter->Append(".png");
	create_task(filePicker->PickSingleFileAsync()).then([this](task<Windows::Storage::StorageFile^> thisTask)
	{
		Windows::Storage::StorageFile ^thisFile;
		try
		{
			thisFile = thisTask.get();
		}
		catch (Exception^)
		{
			return;
		}
		if (thisFile == nullptr)
			return;

		BGChoose_Button->IsEnabled = false;

		Background_Update(thisFile);
		
		thisFile->CopyAsync(Windows::Storage::ApplicationData::Current->LocalFolder);
		Editor_Tools::WriteInAppFile("User_Files", "Background", thisFile->Name);
		BGChoose_Button->IsEnabled = true;;
	}, task_continuation_context::use_current());
}

void MainPage::Background_Update(Windows::Storage::StorageFile ^thisFile)
{
	create_task(thisFile->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([this, thisFile](task<Windows::Storage::Streams::IRandomAccessStream^> readTask)
		{
			auto bgImage = ref new Media::Imaging::BitmapImage();
			bgImage->AutoPlay = true;
			try
			{
				bgImage->SetSource(readTask.get());
			}
			catch (Exception^)
			{
				bgImage->UriSource = ref new Windows::Foundation::Uri("ms-appx:///Assets/Background.png");
			}
			BackgroundIMG->Source = bgImage;
		}, task_continuation_context::use_current());
}
