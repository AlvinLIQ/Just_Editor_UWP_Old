﻿//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------
#include "pch.h"

#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BINDING_DEBUG_OUTPUT
extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
#endif

#include "CodeEditor.xaml.h"
#include "DuronSmartDetect.xaml.h"

void ::Just_Editor::CodeEditor::InitializeComponent()
{
    if (_contentLoaded)
    {
        return;
    }
    _contentLoaded = true;
    ::Windows::Foundation::Uri^ resourceLocator = ref new ::Windows::Foundation::Uri(L"ms-appx:///CodeEditor.xaml");
    ::Windows::UI::Xaml::Application::LoadComponent(this, resourceLocator, ::Windows::UI::Xaml::Controls::Primitives::ComponentResourceLocation::Application);
}

void ::Just_Editor::CodeEditor::Connect(int __connectionId, ::Platform::Object^ __target)
{
    switch (__connectionId)
    {
    case 1:
        {
            this->MainGrid = safe_cast<::Windows::UI::Xaml::Controls::Grid^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Grid^>(this->MainGrid))->KeyDown += ref new ::Windows::UI::Xaml::Input::KeyEventHandler(this, (void (::Just_Editor::CodeEditor::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Input::KeyRoutedEventArgs^))&CodeEditor::MainGrid_KeyDown);
            (safe_cast<::Windows::UI::Xaml::Controls::Grid^>(this->MainGrid))->SizeChanged += ref new ::Windows::UI::Xaml::SizeChangedEventHandler(this, (void (::Just_Editor::CodeEditor::*)
                (::Platform::Object^, ::Windows::UI::Xaml::SizeChangedEventArgs^))&CodeEditor::MainGrid_SizeChanged);
        }
        break;
    case 2:
        {
            this->ExtraColumn = safe_cast<::Windows::UI::Xaml::Controls::ColumnDefinition^>(__target);
        }
        break;
    case 3:
        {
            this->CodeEditorBox = safe_cast<::Windows::UI::Xaml::Controls::RichEditBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::RichEditBox^>(this->CodeEditorBox))->TextChanging += ref new ::Windows::Foundation::TypedEventHandler<::Windows::UI::Xaml::Controls::RichEditBox^, ::Windows::UI::Xaml::Controls::RichEditBoxTextChangingEventArgs^>(this, (void (::Just_Editor::CodeEditor::*)
                (::Windows::UI::Xaml::Controls::RichEditBox^, ::Windows::UI::Xaml::Controls::RichEditBoxTextChangingEventArgs^))&CodeEditor::CodeEditorBox_TextChanging);
            (safe_cast<::Windows::UI::Xaml::Controls::RichEditBox^>(this->CodeEditorBox))->KeyDown += ref new ::Windows::UI::Xaml::Input::KeyEventHandler(this, (void (::Just_Editor::CodeEditor::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Input::KeyRoutedEventArgs^))&CodeEditor::CodeEditorBox_KeyDown);
        }
        break;
    case 4:
        {
            this->SmartDetect = safe_cast<::Just_Editor::DuronSmartDetect^>(__target);
        }
        break;
    case 5:
        {
            this->Pipe_Trans = safe_cast<::Windows::UI::Xaml::Media::TranslateTransform^>(__target);
        }
        break;
    case 6:
        {
            this->Undo_Button = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Button^>(this->Undo_Button))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Just_Editor::CodeEditor::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&CodeEditor::Undo_Button_Click);
        }
        break;
    case 7:
        {
            this->Redo_Button = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Button^>(this->Redo_Button))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Just_Editor::CodeEditor::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&CodeEditor::Redo_Button_Click);
        }
        break;
    case 8:
        {
            this->Save_Button = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Button^>(this->Save_Button))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Just_Editor::CodeEditor::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&CodeEditor::Save_Button_Click);
        }
        break;
    case 9:
        {
            this->Caesar_Button = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Button^>(this->Caesar_Button))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Just_Editor::CodeEditor::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&CodeEditor::Caesar_Button_Click);
        }
        break;
    }
    _contentLoaded = true;
}

::Windows::UI::Xaml::Markup::IComponentConnector^ ::Just_Editor::CodeEditor::GetBindingConnector(int __connectionId, ::Platform::Object^ __target)
{
    __connectionId;         // unreferenced
    __target;               // unreferenced
    return nullptr;
}

#pragma warning(pop)


