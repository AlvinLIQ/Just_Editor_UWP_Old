﻿#pragma once
//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------


namespace Windows {
    namespace UI {
        namespace Xaml {
            namespace Controls {
                ref class Grid;
                ref class ColumnDefinition;
                ref class RichEditBox;
                ref class Button;
            }
        }
    }
}

namespace Just_Editor
{
    [::Windows::Foundation::Metadata::WebHostHidden]
    partial ref class CodeEditor : public ::Windows::UI::Xaml::Controls::Page, 
        public ::Windows::UI::Xaml::Markup::IComponentConnector,
        public ::Windows::UI::Xaml::Markup::IComponentConnector2
    {
    public:
        void InitializeComponent();
        virtual void Connect(int connectionId, ::Platform::Object^ target);
        virtual ::Windows::UI::Xaml::Markup::IComponentConnector^ GetBindingConnector(int connectionId, ::Platform::Object^ target);
    
    private:
        bool _contentLoaded;
    
    
        private: ::Windows::UI::Xaml::Controls::Grid^ MainGrid;
        private: ::Windows::UI::Xaml::Controls::ColumnDefinition^ ExtraColumn;
        private: ::Windows::UI::Xaml::Controls::RichEditBox^ CodeEditorBox;
        private: ::Windows::UI::Xaml::Controls::Button^ Undo_Button;
        private: ::Windows::UI::Xaml::Controls::Button^ Redo_Button;
        private: ::Windows::UI::Xaml::Controls::Button^ Save_Button;
        private: ::Windows::UI::Xaml::Controls::Button^ Caesar_Button;
    };
}

