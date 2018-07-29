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
                ref class SplitView;
                ref class Grid;
                ref class StackPanel;
                ref class Frame;
                ref class Button;
            }
        }
    }
}

namespace Just_Editor
{
    [::Windows::Foundation::Metadata::WebHostHidden]
    partial ref class MainPage : public ::Windows::UI::Xaml::Controls::Page, 
        public ::Windows::UI::Xaml::Markup::IComponentConnector,
        public ::Windows::UI::Xaml::Markup::IComponentConnector2
    {
    public:
        void InitializeComponent();
        virtual void Connect(int connectionId, ::Platform::Object^ target);
        virtual ::Windows::UI::Xaml::Markup::IComponentConnector^ GetBindingConnector(int connectionId, ::Platform::Object^ target);
    
    private:
        bool _contentLoaded;
    
    
        private: ::Windows::UI::Xaml::Controls::SplitView^ SettingsSplit;
        private: ::Windows::UI::Xaml::Controls::Grid^ TopBar_Grid;
        private: ::Windows::UI::Xaml::Controls::StackPanel^ HiddenWindowPanel;
        private: ::Windows::UI::Xaml::Controls::Frame^ MainFrame;
        private: ::Windows::UI::Xaml::Controls::Button^ SettingsButton;
        private: ::Windows::UI::Xaml::Controls::StackPanel^ ToolBar1;
        private: ::Windows::UI::Xaml::Controls::StackPanel^ TopStackPanel;
        private: ::Windows::UI::Xaml::Controls::Button^ GetHiddenWindow_Button;
        private: ::Windows::UI::Xaml::Controls::StackPanel^ WindowPanel;
        private: ::Windows::UI::Xaml::Controls::Button^ AddWindow_Button;
        private: ::Windows::UI::Xaml::Controls::Button^ HomeButton;
    };
}

