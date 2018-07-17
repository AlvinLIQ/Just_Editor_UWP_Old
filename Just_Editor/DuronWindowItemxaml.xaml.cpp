﻿//
// DuronWindowItemxaml.xaml.cpp
// Implementation of the DuronWindowItemxaml class
//

#include "pch.h"
#include "DuronWindowItemxaml.xaml.h"
#include "Editor_Tools.h"

using namespace Just_Editor;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

DuronWindowItemxaml::DuronWindowItemxaml()
{
	InitializeComponent();
}


void Just_Editor::DuronWindowItemxaml::MainGrid_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (!this->isSelected)
	{
		((Windows::UI::Xaml::Controls::Panel^)sender)->Background = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Gray);
	}
}


void Just_Editor::DuronWindowItemxaml::MainGrid_PointerCaptureLost(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (!this->isSelected)
	{
		((Windows::UI::Xaml::Controls::Panel^)sender)->Background = ref new Windows::UI::Xaml::Media::SolidColorBrush;
	}
}


void Just_Editor::DuronWindowItemxaml::CloseWindow_Button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	e->Handled = true;
}

