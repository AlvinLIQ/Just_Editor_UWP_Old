//
// NewFileDialog.xaml.h
// Declaration of the NewFileDialog class
//

#pragma once

#include "NewFileDialog.g.h"

namespace Just_Editor
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class NewFileDialog sealed
	{
	public:
		NewFileDialog();
		property Platform::String^ FileName;
	private:
		//void ContentDialog_PrimaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args);
		//void ContentDialog_SecondaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args);
		void CloseButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void CreateFile_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void FileTypeList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void FileName_TextBox_TextChanging(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxTextChangingEventArgs^ args);
	};
}
