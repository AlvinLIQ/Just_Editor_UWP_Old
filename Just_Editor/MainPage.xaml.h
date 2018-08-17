//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "DuronWindowItemxaml.xaml.h"

namespace Just_Editor
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();
		void OpenFromStorageFile(Windows::Storage::StorageFile^ thisFile, bool AutoSelect);
		void ChangeHiddenPanelExpand();
	private:
		void InitializePage();
		void CheckWindowItem();
		void NewWindowItem(Platform::String^ File_Name, Platform::String^ File_Path, bool AutoSelect = true, Platform::Object^ Frame_Content = nullptr, Windows::Storage::StorageFile^ Item_File = nullptr, bool isChanged = false);
		void WindowSelectAt(int Item_Index);
		void WindowUnSelectAt(int Item_Index);
		void WindowUnSelectAll();
		int GetSelectedItemIndex();
		int GetWindowItemIndex(DuronWindowItemxaml^ sender, Windows::UI::Xaml::Controls::Panel^ thisWindowPanel);
		void MainFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		void AddWindow_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void WindowItemCloseButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void WindowItem_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void WindowItem_Pressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void WindowItem_Released(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void RemoveWindowItem(DuronWindowItemxaml^ sender);
		void SettingsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void HomeButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void GetHiddenWindow_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void WindowPanel_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		void WindowItem_RightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e);
		void Switch_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Page_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
	};
}
