#pragma once
const wchar_t UnabledWords[] = L"\\/:*?<>|";

namespace Just_Editor
{
	class Editor_Tools
	{
	public:
		Editor_Tools();
		~Editor_Tools();
		/*
		Windows::UI::Xaml::Controls::Grid ^ WindowItemTemplate(Platform::String ^ title, Platform::String ^ path)
		{
			auto WindowItemGrid = ref new Windows::UI::Xaml::Controls::Grid;
			WindowItemGrid->Width = 100;
			WindowItemGrid->Name = path;
			WindowItemGrid->Padding = Windows::UI::Xaml::Thickness(2, 0, 0, 0);
			//WindowItemGrid->BorderThickness = Windows::UI::Xaml::Thickness(0, 0, 0.3, 0);
			//WindowItemGrid->BorderBrush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Gray);
			//WindowItemGrid->Background = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::White);

			//Title
			auto TitleBlock = ref new Windows::UI::Xaml::Controls::TextBlock;
			TitleBlock->Text = title;
			TitleBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
			TitleBlock->Foreground = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::DarkSlateGray);
			TitleBlock->TextTrimming = Windows::UI::Xaml::TextTrimming::WordEllipsis;

			TitleBlock->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
			TitleBlock->Margin = Windows::UI::Xaml::Thickness(0, 0, 20, 0);

			//Close Button
			auto CloseButton = ref new Windows::UI::Xaml::Controls::Button;
			CloseButton->VerticalContentAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
			CloseButton->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Stretch;
			CloseButton->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Right;

			CloseButton->Background = ref new Windows::UI::Xaml::Media::SolidColorBrush;
			CloseButton->Foreground = TitleBlock->Foreground;
			CloseButton->BorderThickness = Windows::UI::Xaml::Thickness(0);
			CloseButton->Padding = Windows::UI::Xaml::Thickness(0);

			CloseButton->Width = 20;
			//CloseButton->Height = 20;
			CloseButton->FontWeight = Windows::UI::Text::FontWeights::Bold;
			CloseButton->FontSize = 16;
			CloseButton->Content = "x";

			//Append
			WindowItemGrid->Children->Append(TitleBlock);
			WindowItemGrid->Children->Append(CloseButton);

			return WindowItemGrid;
		}

		bool WindowItem_IsSelectAt(Platform::Object^ sender)
		{
			return ((Windows::UI::Xaml::Controls::Panel^)sender)->Background != nullptr && ((Windows::UI::Xaml::Media::SolidColorBrush^)((Windows::UI::Xaml::Controls::Panel^)sender)->Background)->Color.B == Windows::UI::Colors::White.B;
		}
		*/


		static concurrency::task<Platform::String^> ReadFileInAppFolderAsync(Platform::String^ FolderName, Platform::String^ FileName)
		{
			Windows::Storage::StorageFolder^ storageFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
			return concurrency::create_task(storageFolder->CreateFolderAsync(FolderName, Windows::Storage::CreationCollisionOption::OpenIfExists)).then([File_Name = FileName](Windows::Storage::StorageFolder^ thistask)
			{
				return thistask->CreateFileAsync(File_Name, Windows::Storage::CreationCollisionOption::OpenIfExists);
			}).then([](Windows::Storage::StorageFile^ thisFile)
			{
				return Windows::Storage::FileIO::ReadTextAsync(thisFile);
			});
		}

		static void DeleteFloderInAppAsync(Platform::String^ FolderName)
		{
			Windows::Storage::StorageFolder^ storageFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
			concurrency::create_task(storageFolder->CreateFolderAsync(FolderName, Windows::Storage::CreationCollisionOption::ReplaceExisting));
		}

		static void DeleteFileInAppAsync(Platform::String^ FolderName, Platform::String^ FileName)
		{
			Windows::Storage::StorageFolder^ storageFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
			concurrency::create_task(storageFolder->GetFolderAsync(FolderName)).then([File_Name = FileName](Windows::Storage::StorageFolder^ thistask)
			{
				return thistask->GetFileAsync(File_Name);
			}).then([](Windows::Storage::StorageFile^ this_File)
			{
				this_File->DeleteAsync();
			});

		}

		static void WriteInAppFile(Platform::String^ FolderName, Platform::String^ FileName, Platform::String^ WriteString)
		{
			Windows::Storage::StorageFolder^ storageFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
			concurrency::create_task(storageFolder->CreateFolderAsync(FolderName, Windows::Storage::CreationCollisionOption::OpenIfExists)).then([File_Name = FileName](Windows::Storage::StorageFolder^ thistask)
			{
				return thistask->CreateFileAsync(File_Name, Windows::Storage::CreationCollisionOption::OpenIfExists);
			}).then([Write_String = WriteString](Windows::Storage::StorageFile^ thisFile)
			{
				Windows::Storage::FileIO::WriteTextAsync(thisFile, Write_String);
			});
		}

		static void WriteFile(Windows::Storage::StorageFile^ this_File, Platform::String^ Write_Str)
		{
			concurrency::create_task(Windows::Storage::FileIO::WriteTextAsync(this_File, Write_Str));
		}



		static Concurrency::task<Platform::String^> ReadFileAsync(Windows::Storage::StorageFile^ thisFile)
		{
			return Concurrency::create_task(Windows::Storage::FileIO::ReadTextAsync(thisFile));
		}

		static Windows::UI::Xaml::Controls::ContentDialog^ GetContentDialog(Platform::String^ theTitle, Platform::String^ theMessage, bool needSecondaryButton = false, bool needCloseButton = false)
		{
			auto MsgDialog = ref new Windows::UI::Xaml::Controls::ContentDialog;
			MsgDialog->Title = theTitle;
			MsgDialog->Content = theMessage;
			MsgDialog->PrimaryButtonText = L"Yes";

			if(needSecondaryButton)
				MsgDialog->SecondaryButtonText = L"No";

			if (needCloseButton)
				MsgDialog->CloseButtonText = L"Cancel";

			return MsgDialog;
		}

		static void ShowMessageBox(Platform::String^ theTitle, Platform::String^ theMessage)
		{
			auto MsgDialog = GetContentDialog(theTitle, theMessage);

			MsgDialog->ShowAsync();

			delete MsgDialog;
		}

		static Platform::String^ GetFileNameErrorMsg(Platform::String^ FileName)
		{
			wchar_t * FileNameText = (wchar_t*)FileName->Data();
			int j;
			size_t slen = wcslen(FileNameText);

			for (size_t i = 0; i < slen; i++)
			{
				j = 8;
				while (--j >= 0)
					if (FileNameText[i] == UnabledWords[j])
					{
						return "A file name can't \ncontain \\/:*?<>|";
					}
			}
			return slen ? "" : "A file name can't\nbe empty";
		}

		static wchar_t* SubStr(const wchar_t* WCString, size_t start, size_t end = -1)
		{
			size_t length = wcslen(WCString);


			if (end == -1)
				end = length - start;

			wchar_t* newString = new wchar_t[end];

			for (size_t i = 0; i < end; i++)
			{
				newString[i] = WCString[i + start];
			}
			newString[end] = '\0';

			return newString;
		}

		static Platform::String^ SubPStr(Platform::String^ thisStr, size_t start, size_t end = -1)
		{
			auto wStr = SubStr(thisStr->Data(), start, end);
			auto resultStr = ref new Platform::String(wStr);
			delete[] wStr;

			return resultStr;
		}

		static size_t FindStr(const wchar_t* sourceStr, const wchar_t* targetStr, size_t Slength = -1, size_t Tlength = -1)
		{
			if (Slength == -1)
				Slength = wcslen(sourceStr);

			if (Tlength == -1)
				Tlength = wcslen(targetStr);

			size_t findIndex = 0, i;

			for (i = 0; i + Tlength - findIndex < Slength && findIndex < Tlength; i++, findIndex = sourceStr[i] == targetStr[findIndex] ? findIndex + 1 : 0);
			
			return findIndex == Tlength ? i - Tlength + 1 : -1;
		}
	};
}
