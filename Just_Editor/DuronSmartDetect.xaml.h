//
// DuronSmartDetect.xaml.h
// Declaration of the DuronSmartDetect class
//

#pragma once

#include "DuronSmartDetect.g.h"
#include "DuronWordItem.xaml.h"
#include "Editor_Tools.h"
#include "Editor_Data.h"

namespace Just_Editor
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class DuronSmartDetect sealed
	{
	public:
		DuronSmartDetect();
		void UpdateBindings()
		{
			this->Bindings->Update();
		}
		property Editor_Data^ thisData;
		property DuronWordItem^ SelectedItem;
		property Platform::String^ SelectedWord;
		property unsigned int StartIndex;
		property Windows::UI::Text::ITextRange^ wordRange;
		void DetectWordFromStrArray(Windows::UI::Text::ITextRange^ thisRange, bool isHighlight);
		void SelectAt(unsigned int ItemIndex);
		void SelectItem(DuronWordItem^ thisItem);
	};
}
