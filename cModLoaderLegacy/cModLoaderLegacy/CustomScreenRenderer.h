#pragma once
#include "ModLoader.h"
#include "ModHelper.h"

namespace cModLoader {
    ref class ModLoader;
    ref class ModHelper;
    ref class CustomScreenRenderer {
    private:
        static void _OnExitButton(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement);
        static void _OnLoadDllButton(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement);
        static void _OnMouseOver(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement);
        static void _OnMouseOut(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement);
    public:
        static System::String^ ModMenu_TitleText = "Mod List";
        static System::String^ ModMenu_BackBtnText = "Back";
        static System::String^ ModMenu_UpleadModText = "Uplead Mod DLL";
        static System::String^ OpenModMenuButtonText = "Open cModLoader Mod Menu";
        static Terraria::GameContent::UI::Elements::UIList^ modMenuContainer;
        static Terraria::GameContent::UI::Elements::UIList^ modListContainer;
        static Terraria::GameContent::UI::Elements::UIScrollbar^ modListScrollBar;
        static bool _isScrollbarAttached;
        static Terraria::UI::UIState^ customMenuUI;
        static Terraria::GameContent::UI::Elements::UITextPanel<System::String^>^ Button;
        static void UpdateList();
        static void Init();
        static void OnCustomMenuOpen();
        static void Draw(Microsoft::Xna::Framework::Graphics::SpriteBatch^ batch);
    };
}