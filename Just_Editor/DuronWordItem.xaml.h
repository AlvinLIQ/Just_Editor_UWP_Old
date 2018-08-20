//
// DuronWordItem.xaml.h
// Declaration of the DuronWordItem class
//

#pragma once

#include "DuronWordItem.g.h"

namespace Just_Editor
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class DuronWordItem sealed
	{
	public:
		DuronWordItem();
		property bool isSelected;
		property Platform::String^ Identifier;
		property Windows::UI::Xaml::Media::Brush^ OverBrush;
		property int ItemIndex;

		property bool isClass;

		void Select();
		void UnSelect();
	private:
		void Word_PointerOver(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
		void Word_PointerOut(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args);
	};
}
