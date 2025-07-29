#pragma once
#include "Mod.h"
#include "CustomScreenRenderer.h"

namespace cModLoader {

    namespace ModComponents {

        ref class Mod;

        public ref class ModInfoPage : Terraria::UI::UIState {
        internal:
            void _OnMouseOver(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement);
            void _OnMouseOut(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement);
            void _OnExitButton(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement);
            void _OnOtherButton(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement);
        public:
            Terraria::GameContent::UI::Elements::UITextPanel<String^>^ _TitleElement;
            Terraria::GameContent::UI::Elements::UIText^ _TextElement;
            Terraria::GameContent::UI::Elements::UITextPanel<System::String^>^ _BackButton;
            Terraria::GameContent::UI::Elements::UITextPanel<System::String^>^ _NewButton;
            Terraria::GameContent::UI::Elements::UIPanel^ _containerPanel;
            System::String^ TitleText = "Title";
            System::String^ MainText = "This is a mod.";
            System::String^ BackBtnText = "Back";
            System::String^ UpleadModText = "Web Page";
            System::String^ PageUrl = "";
            ModInfoPage(cModLoader::ModComponents::Mod^ mod);

            virtual bool OnExitButton() { return true; }
            virtual bool OnOtherButton() { return true; }
        };
    }
}