//
// DuronWordItem.xaml.cpp
// Implementation of the DuronWordItem class
//

#include "pch.h"
#include "DuronWordItem.xaml.h"
#include "Editor_Tools.h"

using namespace Just_Editor;

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

DuronWordItem::DuronWordItem()
{
	InitializeComponent();
}

void DuronWordItem::Word_PointerOver(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args)
{
	ItemTitle->Foreground = ref new SolidColorBrush(Editor_Tools::GetColorFromHexChar("DEDEDE"));
}

void DuronWordItem::Word_PointerOut(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args)
{
	ItemTitle->Foreground = OverBrush;
}

void DuronWordItem::Select()
{
	MainGrid->Background = ref new SolidColorBrush(Editor_Tools::GetColorFromHexChar("808080"));
	isSelected = true;
}

void DuronWordItem::UnSelect()
{
	MainGrid->Background = ref new SolidColorBrush;
	isSelected = false;
}
