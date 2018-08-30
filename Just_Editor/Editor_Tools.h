#pragma once
#include "MainPage.xaml.h"

#define MaxUndoLimit 1
#define IdentifierNum 45

const wchar_t UnabledWords[] = L"\\/:*?<>|";

namespace Just_Editor
{
	public ref class Position sealed
	{
	public:
		Position();
		property int StartPosition;
		property int EndPosition;
		property bool isMultiline;
	};
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

		static void AddToRecentFile(Windows::Storage::StorageFile^ thisFile)
		{
			concurrency::create_task(Editor_Tools::ReadFileInAppFolderAsync("User_Files", "RecentList")).then([thisFile](Platform::String^ thisString)
			{
				auto Token = Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->Add(thisFile);
				const wchar_t* thisChar = thisString->Data();
				if (Editor_Tools::FindStr(thisChar, Token->Data()) == -1)
					Editor_Tools::WriteInAppFile("User_Files", "RecentList", thisString + Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->Add(thisFile) + "?");
			}, concurrency::task_continuation_context::use_current());
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
			//delete[] wStr;

			return resultStr;
		}

		static size_t FindStr(const wchar_t* sourceStr, const wchar_t* targetStr, size_t Slength = -1, size_t Tlength = -1, size_t startFindIndex = 0)
		{
			if (Slength == -1)
				Slength = wcslen(sourceStr);

			if (Tlength == -1)
				Tlength = wcslen(targetStr);

			size_t findIndex = 0, i;

			for (i = startFindIndex; i + Tlength - findIndex - 1 < Slength && findIndex < Tlength; findIndex = sourceStr[i] == targetStr[findIndex] ? findIndex + 1 : 0, i++);
			return findIndex == Tlength ? i - Tlength : -1;
		}

		static int GetStrNum(const wchar_t* sourceStr, const wchar_t* targetStr, size_t Slength = -1, size_t Tlength = -1)
		{
			int StrNum = 0;

			if (Slength == -1)
				Slength = wcslen(sourceStr);

			if (Tlength == -1)
				Tlength = wcslen(targetStr);

			size_t findIndex = 0, i;

			for (i = 0; i + Tlength - findIndex - 1 < Slength; findIndex = sourceStr[i] == targetStr[findIndex] ? findIndex + 1 : 0, i++)
			{
				if (findIndex == Tlength)
				{
					findIndex = 0;
					StrNum++;
				}
			}
			return StrNum;
		}

		static size_t FindPStr(Platform::String^ sourceStr, Platform::String^ targetStr, size_t Slength = -1, size_t Tlength = -1)
		{
			if (Slength == -1)
				Slength = sourceStr->Length();

			if (Tlength == -1)
				Tlength = targetStr->Length();
			return FindStr(sourceStr->Data(), targetStr->Data(), Slength, Tlength);
		}

		static void StrCpy(wchar_t** source, wchar_t** target)
		{
			size_t tl = wcslen(*target), i;
			for (i = 0; i < tl; i++)
			{
				*source[i] = *target[i];
			}
			*source[i] = '\0';
		}

		static Platform::String^ GetFileNameFromPath(Platform::String^ PSPath)
		{
			wchar_t* wcsPath = (wchar_t*)PSPath->Data();
			for (size_t i = wcslen(wcsPath); i >= 0; --i)
			{
				if (wcsPath[i] == '\\')
				{
					wcsPath = SubStr(wcsPath, i + 1);
					break;
				}
			}
			return ref new Platform::String(wcsPath);
		}

		static Platform::String^ ReplacePStr(Platform::String^ sStr, Platform::String^ tStr, Platform::String^ replaceStr)
		{
			auto sourceStr = std::wstring(sStr->Data());
			auto targetStr = std::wstring(tStr->Data());
			size_t sLen = sourceStr.length(), tIndex = sourceStr.find(targetStr);
			return ref new Platform::String(sourceStr.substr(0, tIndex).c_str()) + replaceStr + ref new Platform::String(sourceStr.substr(tIndex + targetStr.length()).c_str());
		}

		static Platform::String^ GetRegularUrl(Platform::String^ PUrl)
		{
			auto thisUrl = PUrl->Data();

			if (Editor_Tools::FindStr(thisUrl, L"http") != 0 && !(thisUrl[4] == ':' || (thisUrl[4] == 's' && thisUrl[5] == ':')))
			{
				PUrl = "http://" + PUrl;
			}
			else if (thisUrl[6] != '/' || (thisUrl[7] != '/' && thisUrl[4] == 's') && (thisUrl[5] != '/' && thisUrl[3] == 'p'))
			{
				size_t cIndex = Editor_Tools::FindStr(thisUrl, L"/");
				if (cIndex != -1 && cIndex <= 7)
				{
					PUrl = Editor_Tools::ReplacePStr(PUrl, "/", "");
				}
				PUrl = Editor_Tools::ReplacePStr(PUrl, ":", "://");
			}
			return PUrl;
		}

		static void WriteSetting(Platform::String^ ContainerName, Platform::String^ SettingTypeName, Platform::Object^ SettingInfo)
		{
			if (SettingTypeName == nullptr || SettingInfo == nullptr) return;
			Windows::Storage::ApplicationDataContainer^ localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
			Windows::Storage::ApplicationDataContainer^ container =
				Windows::Storage::ApplicationData::Current->LocalSettings->CreateContainer(ContainerName, Windows::Storage::ApplicationDataCreateDisposition::Always);
			if (localSettings->Containers->HasKey(ContainerName))
			{
				auto values = localSettings->Containers->Lookup(ContainerName)->Values;
				if (values->HasKey(SettingTypeName))
					values->Clear();
				
				values->Insert(SettingTypeName, SettingInfo);
			}
		}

		static Platform::Object^ ReadSetting(Platform::String^ ContainerName, Platform::String^ SettingTypeName)
		{
			Windows::Storage::ApplicationDataContainer^ localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
			Windows::Storage::ApplicationDataContainer^ container =
				Windows::Storage::ApplicationData::Current->LocalSettings->CreateContainer(ContainerName, Windows::Storage::ApplicationDataCreateDisposition::Always);
			bool hasContainer = localSettings->Containers->HasKey(ContainerName);
			bool hasSetting = false;
			if (hasContainer)
			{
				auto values = localSettings->Containers->Lookup(ContainerName)->Values;
				hasSetting = values->HasKey(SettingTypeName);
				return hasSetting ? values->Lookup(SettingTypeName) : "";
			}
			return nullptr;
		}

		static int ReadSettingSize(Platform::String^ ContainerName)
		{
			Windows::Storage::ApplicationDataContainer^ localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
			Windows::Storage::ApplicationDataContainer^ container =
				Windows::Storage::ApplicationData::Current->LocalSettings->CreateContainer(ContainerName, Windows::Storage::ApplicationDataCreateDisposition::Always);
			bool hasContainer = localSettings->Containers->HasKey(ContainerName);
			if (hasContainer)
			{
				auto values = localSettings->Containers->Lookup(ContainerName)->Values;
				return values->Size;
			}
			return 0;
		}

		static void DeleteSettingItem(Platform::String^ CSDN_Data_Container, Platform::String^ Data_Item)
		{
			Windows::Storage::ApplicationDataContainer^ localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
			Windows::Storage::ApplicationDataContainer^ container =
				Windows::Storage::ApplicationData::Current->LocalSettings->CreateContainer(CSDN_Data_Container, Windows::Storage::ApplicationDataCreateDisposition::Always);
			bool hasContainer = localSettings->Containers->HasKey(CSDN_Data_Container);
			if (hasContainer)
			{
				auto values = localSettings->Containers->Lookup(CSDN_Data_Container)->Values;
				if (values->HasKey(Data_Item)) { values->Remove(Data_Item); }
			}
		}

		static void DeleteSetting(Platform::String^ CSDN_Data_Container)
		{
			Windows::Storage::ApplicationDataContainer^ localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
			localSettings->DeleteContainer(CSDN_Data_Container);
		}

		static int GetDecFromHexChar(wchar_t HexChar)
		{
			return (int)HexChar - ((int)HexChar >= 65 ? 55 : 48);
		}

		static Windows::UI::Color GetColorFromHexChar(char* colorHex)
		{
			Windows::UI::Color thisColor;

			int Start_Index = 0;
			if (colorHex[0] == L'#')
			{
				Start_Index = 1;
			}
			thisColor.R = GetDecFromHexChar(colorHex[Start_Index++]) * 16 + GetDecFromHexChar(colorHex[Start_Index++]);
			thisColor.G = GetDecFromHexChar(colorHex[Start_Index++]) * 16 + GetDecFromHexChar(colorHex[Start_Index++]);
			thisColor.B = GetDecFromHexChar(colorHex[Start_Index++]) * 16 + GetDecFromHexChar(colorHex[Start_Index]);
			thisColor.A = thisColor.R && thisColor.G && thisColor.B ? 255 : 0;
			
			return thisColor;
		}

		static Windows::UI::Color GetColorFromHexWChar(wchar_t* colorHex)
		{
			Windows::UI::Color thisColor;
			int Start_Index = 0;
			if (colorHex[0] == L'#')
			{
				Start_Index = 1;
			}
			thisColor.R = GetDecFromHexChar(colorHex[Start_Index++]) * 16 + GetDecFromHexChar(colorHex[Start_Index++]);
			thisColor.G = GetDecFromHexChar(colorHex[Start_Index++]) * 16 + GetDecFromHexChar(colorHex[Start_Index++]);
			thisColor.B = GetDecFromHexChar(colorHex[Start_Index++]) * 16 + GetDecFromHexChar(colorHex[Start_Index]);
			thisColor.A = thisColor.R && thisColor.G && thisColor.B ? 255 : 0;

			return thisColor;
		}

		static Windows::UI::Xaml::Controls::TextBlock^ GetTextBlock(Platform::String^ text, double fontSize, Windows::UI::Xaml::Media::Brush^ foreground, Windows::UI::Text::FontWeight fontWeight)
		{
			auto thisItem = ref new Windows::UI::Xaml::Controls::TextBlock;
			thisItem->Text = text;
			thisItem->FontSize = fontSize;
			thisItem->Foreground = foreground;
			thisItem->FontWeight = fontWeight;
			return thisItem;
		}

		static Windows::UI::Xaml::Controls::TextBox^ GetTextBox(Windows::UI::Xaml::Media::Brush^ foreground, bool acceptsReturn = true)
		{
			auto thisItem = ref new Windows::UI::Xaml::Controls::TextBox;
			thisItem->Foreground = foreground;
			thisItem->Background = ref new Windows::UI::Xaml::Media::SolidColorBrush;
			thisItem->FontSize = 15;
			thisItem->Margin = Windows::UI::Xaml::Thickness(0, 10, 0, 10);
			thisItem->AcceptsReturn = acceptsReturn;
			thisItem->IsSpellCheckEnabled = false;

			return thisItem;
		}

		static Windows::UI::Xaml::Controls::Button^ GetButton(Platform::Object^ content, double fontSize, Windows::UI::Xaml::Media::Brush^ background, Windows::UI::Xaml::Media::Brush^ foreground, Windows::UI::Text::FontWeight fontWeight)
		{
			auto thisItem = ref new Windows::UI::Xaml::Controls::Button;
			thisItem->Background = background;
			thisItem->Content = content;
			thisItem->FontSize = fontSize;
			thisItem->Foreground = foreground;
			thisItem->FontWeight = fontWeight;
			thisItem->Margin = Windows::UI::Xaml::Thickness(0, 10, 0, 10);
			return thisItem;
		}
	};
}
