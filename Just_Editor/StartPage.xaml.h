//
// StartPage.xaml.h
// Declaration of the StartPage class
//

#pragma once

#include "StartPage.g.h"
#include "RecentListItem.xaml.h"

namespace Just_Editor
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class StartPage sealed
	{	
	public:
		StartPage();
	private:
		void LoadRecentList();
		void Page_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		void NewOptionView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void OpenOptionView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		int GetRecentItemIndex(RecentListItem^ sender, Windows::UI::Xaml::Controls::Panel^ thisWindowPanel);
	};
}
