//
// RecentListItem.xaml.h
// Declaration of the RecentListItem class
//

#pragma once

#include "RecentListItem.g.h"

namespace Just_Editor
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class RecentListItem sealed
	{
	public:
		RecentListItem();
		property Platform::String^ FileName;
		property Platform::String^ FilePath;

		property Platform::String^ Token;

	private:
		void MainGrid_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void MainGrid_PointerCaptureLost(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Remove_Button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
	};
}
