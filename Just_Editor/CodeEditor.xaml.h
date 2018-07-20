//
// CodeEditor.xaml.h
// Declaration of the CodeEditor class
//

#pragma once

#include "CodeEditor.g.h"
#include "DuronWindowItemxaml.xaml.h"

namespace Just_Editor
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CodeEditor sealed
	{
	public:
		CodeEditor();

		property DuronWindowItemxaml^ thisWindowItem;

		void ThisFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
	private:
		void CodeEditorBox_TextChanging(Windows::UI::Xaml::Controls::RichEditBox^ sender, Windows::UI::Xaml::Controls::RichEditBoxTextChangingEventArgs^ args);
		void CodeEditorBox_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void Undo_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Redo_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
