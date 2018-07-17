//
// NewFileDialog.xaml.cpp
// Implementation of the NewFileDialog class
//

#include "pch.h"
#include "NewFileDialog.xaml.h"

const wchar_t UnabledWords[] = L"\\/:*?<>|";

using namespace Just_Editor;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Content Dialog item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

Just_Editor::NewFileDialog::NewFileDialog()
{
	InitializeComponent();
}

void Just_Editor::NewFileDialog::CloseButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	FileName = "";
	this->Hide();
}


void Just_Editor::NewFileDialog::CreateFile_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (ErrorMsg_Block->Text == "")
	{
		FileName = FileName_TextBox->Text;
		this->Hide();
	}
}


void Just_Editor::NewFileDialog::FileTypeList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	FileName_TextBox->IsEnabled = true;
	CreateFile_Button->IsEnabled = true;
}


void Just_Editor::NewFileDialog::FileName_TextBox_TextChanging(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxTextChangingEventArgs^ args)
{
	wchar_t * FileNameText = (wchar_t*)sender->Text->Data();
	int j;
	size_t slen = wcslen(FileNameText);

	for (size_t i = 0; i < slen; i++)
	{
		j = 8;
		while(--j >= 0)
			if (FileNameText[i] == UnabledWords[j])
			{
				ErrorMsg_Block->Text = "A file name can't \ncontain \\/:*?<>|";
				return;
			}
	}
	ErrorMsg_Block->Text = slen ? "" : "A file name can't\nbe empty";
}
