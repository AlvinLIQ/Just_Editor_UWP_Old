//
// CaesarPanel.xaml.h
// Declaration of the CaesarPanel class
//

#pragma once

#include "CaesarPanel.g.h"
#include "Editor_Data.h"

namespace Just_Editor
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CaesarPanel sealed
	{
	public:
		CaesarPanel();
		property Editor_Data^ thisData;
		void SetPanelMode(int PanelMode);
		void UpdateBindings()
		{
			this->Bindings->Update();
		}
	private:
		void Post_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Calculate_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void AddParameter_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void AddParameter();
		void PanelTitle_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void PanelTitle_PointerCaptureLost(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void PanelTitle_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void CipherType_List_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void InitializePanel();
	};
}
