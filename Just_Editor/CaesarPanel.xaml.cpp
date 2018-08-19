//
// CaesarPanel.xaml.cpp
// Implementation of the CaesarPanel class
//

#include "pch.h"
#include "CaesarPanel.xaml.h"
#include "Editor_Tools.h"

using namespace Just_Editor;

using namespace Platform;
using namespace concurrency;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Web::Http;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236
Button^ Calculate_Button;
TextBox^ Cipher_Box;
TextBlock^ Result_Block;
String^ PanelTitleArray[] = {"Caesar","Request","WebView"};

CaesarPanel::CaesarPanel()
{
	InitializeComponent();
	InitializePanel();
}

void Just_Editor::CaesarPanel::InitializePanel()
{
	thisData = ref new Editor_Data;
	for (int i = 0; i < 3; i++)
	{
		auto PanelType_Block = ref new TextBlock;
		PanelType_Block->Foreground = thisData->Editor_ForegroundBrush;
		PanelType_Block->Text = PanelTitleArray[i];

		auto thisItem = ref new GridViewItem;
		thisItem->Content = PanelType_Block;
		thisItem->Width = 70;

		CipherType_List->Items->Append(thisItem);
	}

	
//	CipherType_List->
}

void Just_Editor::CaesarPanel::SetPanelMode(int PanelMode)
{
	CipherType_List->Width = 0;
	thisWebView->Width = 0;
	thisWebView->Opacity = 0;
	MainPanel->Children->Clear();
	thisData = ref new Editor_Data;
	this->Bindings->Update();
	PanelTitle->Text = PanelTitleArray[PanelMode];
	switch (PanelMode)
	{
	case 0://Caesar
	{
		Cipher_Box = Editor_Tools::GetTextBox(thisData->Editor_ForegroundBrush);
		Cipher_Box->BorderBrush = thisData->Editor_BorderBrush;
		Cipher_Box->MinHeight = 80;

		Calculate_Button = Editor_Tools::GetButton("Calculate", 16, thisData->Editor_BackgroundBrush ,thisData->Editor_ForegroundBrush, PanelTitle->FontWeight);
		Calculate_Button->Click += ref new RoutedEventHandler(this, &Just_Editor::CaesarPanel::Calculate_Button_Click);

		Result_Block = ref new TextBlock;
		Result_Block->Foreground = thisData->Editor_ForegroundBrush;
		Result_Block->FontSize = 15;
		Result_Block->IsTextSelectionEnabled = true;

		MainPanel->Children->Append(Editor_Tools::GetTextBlock("Cipher", 16, thisData->Editor_ForegroundBrush, PanelTitle->FontWeight));
		MainPanel->Children->Append(Cipher_Box);
		MainPanel->Children->Append(Calculate_Button);
		MainPanel->Children->Append(Editor_Tools::GetTextBlock("Result", 16, thisData->Editor_ForegroundBrush, PanelTitle->FontWeight));
		MainPanel->Children->Append(Result_Block);
	}
		break;
	case 1://Request
	{
		Cipher_Box = Editor_Tools::GetTextBox(thisData->Editor_ForegroundBrush);
		Cipher_Box->BorderBrush = thisData->Editor_BorderBrush;
		Cipher_Box->MinHeight = 140;

		auto Url_Box = Editor_Tools::GetTextBox(thisData->Editor_ForegroundBrush, false);
		Url_Box->BorderBrush = thisData->Editor_BorderBrush;

		auto Body_Box = Editor_Tools::GetTextBox(thisData->Editor_ForegroundBrush, false);
		Body_Box->BorderBrush = thisData->Editor_BorderBrush;

		Result_Block = ref new TextBlock;
		Result_Block->Foreground = thisData->Editor_ForegroundBrush;
		Result_Block->FontSize = 15;
		Result_Block->IsTextSelectionEnabled = true;

		Calculate_Button = Editor_Tools::GetButton("Execute", 16, thisData->Editor_BackgroundBrush, thisData->Editor_ForegroundBrush, PanelTitle->FontWeight);
		Calculate_Button->Click += ref new Windows::UI::Xaml::RoutedEventHandler([this, Url_Box, Body_Box] (Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
		{
			Url_Box->Text = Editor_Tools::GetRegularUrl(Url_Box->Text);

			auto thisClient = ref new HttpClient;
			auto thisContent = ref new HttpStringContent(Body_Box->Text);

			thisClient->DefaultRequestHeaders->Clear();
			
			std::wstring RequestHeader = (Cipher_Box->Text + "\r")->Data();
			size_t CIndex = RequestHeader.find(L":"), EIndex;
			String^ thisValue, ^thisName;

			while (CIndex != -1)
			{
				EIndex = RequestHeader.find(L"\r");
				thisName = ref new String(RequestHeader.substr(0, CIndex).c_str());
				thisValue = ref new String(RequestHeader.substr(CIndex + 1, EIndex - CIndex - 1).c_str());
				try
				{
					thisClient->DefaultRequestHeaders->Append(thisName, thisValue);
				}
				catch (Exception^)
				{
					if (thisName == L"Accept")
					{
						thisClient->DefaultRequestHeaders->Accept->Clear();
						thisClient->DefaultRequestHeaders->Accept->Append(ref new Headers::HttpMediaTypeWithQualityHeaderValue(thisValue));
					}
					else if (thisName == L"Accept-Encoding")
					{
						thisClient->DefaultRequestHeaders->AcceptEncoding->Clear();
						thisClient->DefaultRequestHeaders->AcceptEncoding->Append(ref new Headers::HttpContentCodingWithQualityHeaderValue(thisValue));
					}
					else if (thisName == L"User-Agent")
					{
						thisClient->DefaultRequestHeaders->UserAgent->Clear();
						thisClient->DefaultRequestHeaders->UserAgent->Append(ref new Headers::HttpProductInfoHeaderValue(thisValue));
					}
					else if (thisName == L"Connection")
					{
						thisClient->DefaultRequestHeaders->Connection->Clear();
						thisClient->DefaultRequestHeaders->Connection->Append(ref new Headers::HttpConnectionOptionHeaderValue(thisValue));
					}
					else if (thisName == L"Host")
						thisClient->DefaultRequestHeaders->Host = ref new Windows::Networking::HostName(thisValue);
					else if (thisName == L"Referer")
						thisClient->DefaultRequestHeaders->Referer = ref new Windows::Foundation::Uri(thisValue);
					else if (thisName == L"From")
						thisClient->DefaultRequestHeaders->From = thisValue;
					else if (thisName == L"Content-Type")
						thisContent->Headers->ContentType = ref new Headers::HttpMediaTypeHeaderValue(thisValue);
					else if (thisName == L"Content-Length")
					{
						IBox<unsigned long long>^ thisLength = (unsigned long long)_wtoi(thisValue->Data());
						thisContent->Headers->ContentLength = thisLength;
					}
				}


				RequestHeader = RequestHeader.substr(EIndex + 1);
				CIndex = RequestHeader.find(L":"), EIndex;
			}
			//thisClient->DefaultRequestHeaders->
			create_task(thisClient->PostAsync(ref new Windows::Foundation::Uri(Url_Box->Text), thisContent)).then([this] (task<HttpResponseMessage^> thisTask) 
			{
				HttpResponseMessage^ thisResponse;
				try
				{
					thisResponse = thisTask.get();
				}
				catch (Exception^)
				{
					return;
				}
				create_task(thisResponse->Content->ReadAsStringAsync()).then([this] (task<String^> getTask)
				{
					try
					{
						Result_Block->Text = getTask.get();
					}
					catch (Exception^)
					{
						return;
					}
				}, task_continuation_context::use_current());
			});
		});

		MainPanel->Children->Append(Editor_Tools::GetTextBlock("Url", 16, thisData->Editor_ForegroundBrush, PanelTitle->FontWeight));
		MainPanel->Children->Append(Url_Box);
		MainPanel->Children->Append(Editor_Tools::GetTextBlock("Header", 16, thisData->Editor_ForegroundBrush, PanelTitle->FontWeight));
		MainPanel->Children->Append(Cipher_Box);
		MainPanel->Children->Append(Editor_Tools::GetTextBlock("Body(Post)", 16, thisData->Editor_ForegroundBrush, PanelTitle->FontWeight));
		MainPanel->Children->Append(Body_Box);
		MainPanel->Children->Append(Calculate_Button);
		MainPanel->Children->Append(Editor_Tools::GetTextBlock("Result", 16, thisData->Editor_ForegroundBrush, PanelTitle->FontWeight));
		MainPanel->Children->Append(Result_Block);
	}
		break;
	case 2://WebView
		Calculate_Button = Editor_Tools::GetButton("Navigate From Editor", 16, thisData->Editor_BackgroundBrush, thisData->Editor_ForegroundBrush, PanelTitle->FontWeight);
		Calculate_Button->Click += ref new Windows::UI::Xaml::RoutedEventHandler([this](Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
		{
			thisWebView->NavigateToString(((RichEditBox^)((Grid^)((Grid^)this->Parent)->Children->GetAt(1))->Children->GetAt(0))->Document->GetRange(0, Windows::UI::Text::TextConstants::MaxUnitCount)->Text);
		});
		MainPanel->Children->Append(Calculate_Button);
		thisWebView->Width = Width;
		thisWebView->Opacity = 1;
		break;
	}
}
//Caesar
void Just_Editor::CaesarPanel::Calculate_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (Cipher_Box->Text == "")
		return;
	Result_Block->Text = "";

	wchar_t* CipherText = (wchar_t*)Cipher_Box->Text->Data();
	int j, k, cipherSize = (int)wcslen(CipherText);
	for (int i = 0; i < 25; i++)
	{
		for (j = 0; j < cipherSize; j++)
		{
			if (CipherText[j] < 65 || CipherText[j] > 122 || (CipherText[j] > 90 && CipherText[j] < 97))
				continue;

			k = CipherText[j] >= 97 ? 97 : 65;
			CipherText[j] = (CipherText[j] - k + 1) % 26 + k;
		}
		Result_Block->Text += ref new String(CipherText) + "\n\n";
	}

	//delete[] CipherText;
}
//Post Request
void Just_Editor::CaesarPanel::Post_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}

void Just_Editor::CaesarPanel::AddParameter_Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	AddParameter();
}

void Just_Editor::CaesarPanel::AddParameter()
{

}

void Just_Editor::CaesarPanel::PanelTitle_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	PanelTitle->Foreground = thisData->Editor_BackgroundBrush;
}


void Just_Editor::CaesarPanel::PanelTitle_PointerCaptureLost(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	PanelTitle->Foreground = thisData->Editor_ForegroundBrush;
}


void Just_Editor::CaesarPanel::PanelTitle_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	CipherType_List->Width = Width;
}


void Just_Editor::CaesarPanel::CipherType_List_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (CipherType_List->SelectedIndex >= 0)
	{
		SetPanelMode(CipherType_List->SelectedIndex);
		CipherType_List->SelectedItem = nullptr;
	}
}

