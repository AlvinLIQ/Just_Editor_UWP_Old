//
// RecentListItem.xaml.cpp
// Implementation of the RecentListItem class
//

#include "pch.h"
#include "RecentListItem.xaml.h"

using namespace Just_Editor;

using namespace Platform;
using namespace Windows::UI::Xaml::Media;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

RecentListItem::RecentListItem()
{
	InitializeComponent();
}


void Just_Editor::RecentListItem::MainGrid_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	this->Background = Foreground;

}


void Just_Editor::RecentListItem::MainGrid_PointerCaptureLost(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	this->Background = ref new Windows::UI::Xaml::Media::SolidColorBrush;
}


void Just_Editor::RecentListItem::Remove_Button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	e->Handled = true;
}
