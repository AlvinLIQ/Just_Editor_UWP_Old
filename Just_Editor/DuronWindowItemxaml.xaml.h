//
// DuronWindowItemxaml.xaml.h
// Declaration of the DuronWindowItemxaml class
//

#pragma once

#include "DuronWindowItemxaml.g.h"
#include "Editor_Data.h"
#include "Editor_Tools.h"

namespace Just_Editor
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class DuronWindowItemxaml sealed
	{
	public:
		DuronWindowItemxaml();
		property Editor_Data^ thisData;
		property Platform::String^ FilePath;
		property Platform::String^ FileName;

		property Windows::Storage::StorageFile^ ItemFile;

		property Platform::Object^ FrameContent;

		property bool isSelected;

		property int isChanged;

		void UpdateBindings()
		{
			this->Bindings->Update();
		}

		void Unselect()
		{
			Background = ref new Windows::UI::Xaml::Media::SolidColorBrush;
			isSelected = false;
		}

		void Select()
		{
			Background = thisData->ToolBar_BackgroundBrush;
			isSelected = true;
		}

		void SetChanged(int Parameter)
		{
			isChanged = Parameter;
			isChangedName_Block->Opacity = Parameter;
		}

		void SetFileName(Platform::String^ newFileName);
		void SetDisplayName(Platform::String^ newDisplayName);
	private:
		void MainGrid_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void MainGrid_PointerCaptureLost(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void CloseWindow_Button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
	};
}
