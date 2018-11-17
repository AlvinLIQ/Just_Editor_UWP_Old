//
// NewFileDialog.xaml.cpp
// Implementation of the NewFileDialog class
//

#include "pch.h"
#include "NewFileDialog.xaml.h"
#include "Editor_Tools.h"

using namespace Just_Editor;

using namespace Platform;
using namespace Windows::UI::Xaml::Controls;

// The Content Dialog item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

String^ File_Type[] = {L".c", L".cpp", L".cs", L".java", L"html", L".py"};

Just_Editor::NewFileDialog::NewFileDialog()
{
	InitializeComponent();
	/*
				<ListViewItem>
					<StackPanel Orientation="Horizontal">
						<Image Source="Assets/New_Icon.png" Width="30"/>
						<TextBlock Text=" Text" Foreground="#5D5B5B" FontWeight="Bold" VerticalAlignment="Center"/>
					</StackPanel>
				</ListViewItem>
	*/
	for (int i = 0; i < 6; i++)
	{
		auto thisIcon = ref new Image;
		thisIcon->Width = 30;
		thisIcon->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Windows::Foundation::Uri("ms-appx:///Assets/Script_Icon.png"));
		thisIcon->Margin = Windows::UI::Xaml::Thickness(0, 0, 10, 0);
		
		auto thisPanel = ref new StackPanel;
		thisPanel->Orientation = Orientation::Horizontal;
		thisPanel->Children->Append(thisIcon);
		thisPanel->Children->Append(Editor_Tools::GetTextBlock(File_Type[i], FontSize, DialogTitle->Foreground, Windows::UI::Text::FontWeights::Bold));
		
		FileTypeList->Items->Append(thisPanel); 
	}
}

void Just_Editor::NewFileDialog::CloseButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	FileName = "";
	this->Hide();
}


void Just_Editor::NewFileDialog::CreateFile_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (ErrorMsg_Block->Text == "" && FileTypeList->SelectedIndex != -1)
	{
		FileName = FileName_TextBox->Text + File_Type[FileTypeList->SelectedIndex];
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
	ErrorMsg_Block->Text = Editor_Tools::GetFileNameErrorMsg(sender->Text);
}
