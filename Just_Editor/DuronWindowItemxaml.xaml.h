//
// DuronWindowItemxaml.xaml.h
// Declaration of the DuronWindowItemxaml class
//

#pragma once

#include "DuronWindowItemxaml.g.h"

namespace Just_Editor
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class DuronWindowItemxaml sealed
	{
	public:
		DuronWindowItemxaml();
		property Platform::String^ FilePath;
		property Platform::String^ FileName;
		property Platform::String^ OriginalText;

		property Windows::Storage::StorageFile^ ItemFile;

		property Platform::Object^ FrameContent;

		property bool isSelected;

		property bool isChanged;

		void Unselect()
		{
			Background = ref new Windows::UI::Xaml::Media::SolidColorBrush;
			isSelected = false;
		}

		void Select()
		{
			Background = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::WhiteSmoke);
			isSelected = true;
		}

		void SetChanged(bool Parameter)
		{
			if (Parameter == isChanged) return;

			wchar_t* tempName = (wchar_t*)FileName_Block->Text->Data();
			size_t lengthOfName = wcslen(tempName) - 1;
			if (tempName[lengthOfName] != '*' && Parameter)
			{
				FileName_Block->Text += L"*";
			}
			else
			{
				FileName_Block->Text = FileName;
				//delete[] tempName;
			}

			isChanged = Parameter;
		}

		void SetFileName(Platform::String^ newFileName);
	private:
		void MainGrid_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void MainGrid_PointerCaptureLost(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void CloseWindow_Button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
	};
}
