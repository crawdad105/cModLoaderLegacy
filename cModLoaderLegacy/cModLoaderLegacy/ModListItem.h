#pragma once
#include "Mod.h"

namespace cModLoader {
    namespace ModComponents {
        
        ref class Mod;

        public ref class ModListItem : Terraria::GameContent::UI::Elements::UIPanel {
        private:
            cModLoader::ModComponents::Mod^ modRefrence;
            Microsoft::Xna::Framework::Graphics::Texture2D^ _ModImage;
            ReLogic::Content::Asset<Microsoft::Xna::Framework::Graphics::Texture2D^>^ _dividerTexture;
            ReLogic::Content::Asset<Microsoft::Xna::Framework::Graphics::Texture2D^>^ _innerPanelTexture;
            ReLogic::Content::Asset<Microsoft::Xna::Framework::Graphics::Texture2D^>^ _infoIcon;
            ReLogic::Content::Asset<Microsoft::Xna::Framework::Graphics::Texture2D^>^ _openFileIcon;
            ReLogic::Content::Asset<Microsoft::Xna::Framework::Graphics::Texture2D^>^ _configIcon;
        public:
            Terraria::UI::UIElement^ _playerPanel;
            Terraria::GameContent::UI::Elements::UIImageButton^ _OpenFileButton;
            Terraria::GameContent::UI::Elements::UIImageButton^ _OpenPageInfoButton;
            Terraria::GameContent::UI::Elements::UIImageButton^ _OpenConfigButton;
            Terraria::GameContent::UI::Elements::UIText^ _TitleElement;
            Terraria::GameContent::UI::Elements::UIText^ _AuthorElement;
            Terraria::GameContent::UI::Elements::UIText^ _VersionElement;
            void DrawPanel(Microsoft::Xna::Framework::Graphics::SpriteBatch^ spriteBatch, Microsoft::Xna::Framework::Vector2 position, float width);
            ModListItem(cModLoader::ModComponents::Mod^ mod);
        protected:
            void DrawSelf(Microsoft::Xna::Framework::Graphics::SpriteBatch^ spriteBatch) override;
        internal:
            void _OnClickOpenMod(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement);
            void _OnClickModInfo(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement);
            void _OnClickModConfig(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement);
        };
    }
}