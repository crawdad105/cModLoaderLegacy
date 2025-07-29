#include "pch.h"
#include "ModListItem.h"

#include "CommonNamespaces.h"

namespace cModLoader {
    namespace ModComponents {
        
        void ModListItem::DrawPanel(Microsoft::Xna::Framework::Graphics::SpriteBatch^ spriteBatch, Microsoft::Xna::Framework::Vector2 position, float width)
        {
            auto c = Microsoft::Xna::Framework::Color(255, 255, 255);
            auto tex = _innerPanelTexture->Value;
            int h = tex->Height;
            spriteBatch->Draw(tex, position, Microsoft::Xna::Framework::Rectangle(0, 0, 8, h), c);
            spriteBatch->Draw(tex, Microsoft::Xna::Framework::Vector2(position.X + 8, position.Y), Microsoft::Xna::Framework::Rectangle(8, 0, 8, h), c, 0, Microsoft::Xna::Framework::Vector2::Zero, Microsoft::Xna::Framework::Vector2((width - 16) / 8, 1), Microsoft::Xna::Framework::Graphics::SpriteEffects::None, 0);
            spriteBatch->Draw(tex, Microsoft::Xna::Framework::Vector2(position.X + width - 8, position.Y), Microsoft::Xna::Framework::Rectangle(16, 0, 8, h), c);
        }
        ModListItem::ModListItem(Mod^ mod) {

            int ModIconSize = 96;

            modRefrence = mod;
            BorderColor = Color(89, 116, 213) * 0.7f;
            String^ devider = "Images/UI/Divider";
            String^ innerBg = "Images/UI/InnerPanelBackground";
            String^ playerBg = "Images/UI/PlayerBackground";
            String^ infoIcon = "Images/UI/CharCreation/CharInfo";
            String^ openFile = "Images/UI/Camera_6";
            _dividerTexture = Terraria::Main::Assets->Request<Texture2D^>(devider, (AssetRequestMode)1);
            _innerPanelTexture = Terraria::Main::Assets->Request<Texture2D^>(innerBg, (AssetRequestMode)1);
            auto imgPath = mod->GetFullFilePath()->Substring(0, mod->GetFullFilePath()->LastIndexOf(".") + 1) + "png";
            if (System::IO::File::Exists(imgPath)) {
                System::IO::FileStream^ fileStream = gcnew System::IO::FileStream(imgPath, System::IO::FileMode::Open);
                _ModImage = Texture2D::FromStream(Terraria::Main::instance->GraphicsDevice, fileStream);
                fileStream->Close();
            }
            else _ModImage = Terraria::Main::Assets->Request<Texture2D^>(playerBg, (AssetRequestMode)1)->Value;
            _infoIcon = Terraria::Main::Assets->Request<Texture2D^>(infoIcon, (AssetRequestMode)1);
            _openFileIcon = Terraria::Main::Assets->Request<Texture2D^>(openFile, (AssetRequestMode)1);
            Height.Set(ModIconSize, 0);
            Width.Set(0, 1);
            SetPadding(6);
            _playerPanel = gcnew UIElement();
            _playerPanel->Width.Set(59, 0);
            _playerPanel->Height.Set(58, 0);
            _playerPanel->Left.Set(4, 0);
            Append(_playerPanel);

            _OpenFileButton = gcnew UIImageButton(_openFileIcon);
            _OpenFileButton->VAlign = 1;
            _OpenFileButton->HAlign = 1;
            _OpenFileButton->Left.Set(2, 0);
            _OpenFileButton->Top.Set(4, 0);
            _OpenFileButton->OnLeftClick += gcnew Terraria::UI::UIElement::MouseEvent(this, &ModListItem::_OnClickOpenMod);
            Append(_OpenFileButton);

            _OpenPageInfoButton = gcnew UIImageButton(_infoIcon);
            _OpenPageInfoButton->VAlign = 1;
            _OpenPageInfoButton->HAlign = 1;
            _OpenPageInfoButton->Left.Set(-30, 0);
            _OpenPageInfoButton->Top.Set(4, 0);
            _OpenPageInfoButton->OnLeftClick += gcnew Terraria::UI::UIElement::MouseEvent(this, &ModListItem::_OnClickModInfo);
            Append(_OpenPageInfoButton);

            // _OpenConfigButton = gcnew UIImageButton(_configIcon);
            // _OpenConfigButton->VAlign = 1;
            // _OpenConfigButton->HAlign = 1;
            // _OpenConfigButton->Left.Set(-50, 0);
            // _OpenConfigButton->Top.Set(4, 0);
            // _OpenConfigButton->OnLeftClick += gcnew Terraria::UI::UIElement::MouseEvent(this, &ModListItem::_OnClickModConfig);
            // Append(_OpenConfigButton);

            _TitleElement = gcnew UIText(modRefrence->ModName == nullptr ? "ERROR: nullptr" : modRefrence->ModName, 1, false);
            _TitleElement->Left.Set(ModIconSize + 2, 0);
            _TitleElement->Top.Set(4, 0);
            Append(_TitleElement);

            _AuthorElement = gcnew UIText(modRefrence->ModAuthor == nullptr ? "ERROR: nullptr" : modRefrence->ModAuthor, 0.8f, false);
            _AuthorElement->Left.Set(ModIconSize + 4, 0);
            _AuthorElement->Top.Set(36, 0);
            Append(_AuthorElement);

            _VersionElement = gcnew UIText(modRefrence->ModVersion == nullptr ? "ERROR: nullptr" : modRefrence->ModVersion, 0.8f, false);
            _VersionElement->HAlign = 1;
            _VersionElement->Left.Set(-10, 0);
            _VersionElement->Top.Set(36, 0);
            Append(_VersionElement);


        }
        void ModListItem::DrawSelf(SpriteBatch^ sb) {
            UIPanel::DrawSelf(sb);

            auto rect = GetDimensions();
            auto white = Color(255, 255, 255);
            auto srcRecNull = Nullable<Microsoft::Xna::Framework::Rectangle>();

            // draw mod name
            int plrBGPosOff = (rect.Height - _ModImage->Height) / 2;
            auto plrBGPos = Vector2(rect.X + plrBGPosOff, rect.Y + plrBGPosOff);
            sb->Draw(_ModImage, plrBGPos, srcRecNull, white, 0, Vector2::Zero, Vector2(1, 1), SpriteEffects::None, 0);
            // draw seperator
            auto sepPos = Vector2(rect.X + rect.Height, rect.Y + 30);
            sb->Draw(_dividerTexture->Value, sepPos, srcRecNull, white, 0, Vector2::Zero, Vector2((rect.Width - rect.Height) / 8, 1), SpriteEffects::None, 0);
            // draw author
            auto pnlWidth = ((rect.Width - rect.Height) / 2) - 10;
            auto pnlPos1 = Vector2(rect.X + rect.Height + 4, rect.Y + 36);
            DrawPanel(sb, pnlPos1, pnlWidth);
            // draw version
            auto pnlPos2 = pnlPos1 + Vector2(pnlWidth + 10, 0);
            DrawPanel(sb, pnlPos2, pnlWidth);

        }

        void ModListItem::_OnClickOpenMod(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
            System::Diagnostics::Process::Start("explorer.exe", modRefrence->ModFilePath);
        }
        void ModListItem::_OnClickModInfo(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
            Terraria::Audio::SoundEngine::PlaySound(10, -1, -1, 1, 1, 0);
            Terraria::Main::MenuUI->SetState(modRefrence->GetModInfoPage());
        }
        void ModListItem::_OnClickModConfig(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
            Terraria::Audio::SoundEngine::PlaySound(10, -1, -1, 1, 1, 0);
            Terraria::Main::MenuUI->SetState(modRefrence->GetModInfoPage());
        }

    }
}