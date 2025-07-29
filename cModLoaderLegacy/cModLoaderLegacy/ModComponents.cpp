#include "cModLoader.h"
#include "Mod.h"
#include "CustomModInfoPage.h"
#include "ModListItem.h"
#include "ModHelper.h"
#include "ModLoader.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace System::Runtime::InteropServices;

using namespace Terraria::UI;
using namespace Terraria::GameContent::UI;
using namespace Terraria::GameContent::UI::Elements;
using namespace ReLogic::Content;

namespace cModLoader {
    
    namespace ModComponents {

        Mod::Mod(String^ name, String^ desc, String^ author, String^ version, String^ url) { ModName = name; ModDescription = desc; ModAuthor = author; ModVersion = version; ModUrl = url; ModPath = ""; }
        Mod::Mod(String^ name, String^ desc, String^ author, String^ version) { Mod(name, desc, author, version, ""); }
        void Mod::CreateMod() { ModLoader::LoadMod(this); }
    

        CustomModInfoPage::CustomModInfoPage(Mod^ mod) { TitleText = mod->ModName; MainText = mod->ModDescription; PageUrl = mod->ModUrl; }
        void CustomModInfoPage::OnInitialize() {
            UIElement^ uIElement = gcnew UIElement();
            uIElement->Width.Set(0, 0.8f);
            uIElement->MaxWidth.Set(650, 0);
            uIElement->Top.Set(220, 0);
            uIElement->Height.Set(-220, 1);
            uIElement->HAlign = 0.5f;
            UIPanel^ uIPanel = gcnew UIPanel();
            uIPanel->Width.Set(0, 1);
            uIPanel->Height.Set(-110, 1);
            uIPanel->BackgroundColor = Color(33, 43, 79) * 0.8f;
            _containerPanel = uIPanel;
            uIElement->Append(uIPanel);
            UITextPanel<String^>^ uITextPanel = gcnew UITextPanel<String^>(MainText, 0.8f, true);
            uITextPanel->HAlign = 0.5f;
            uITextPanel->Top.Set(-40, 0);
            uITextPanel->SetPadding(15);
            uITextPanel->BackgroundColor = Color(73, 94, 171);
            uIElement->Append(uITextPanel);
            UITextPanel<String^>^ uITextPanel2 = gcnew UITextPanel<String^>(BackBtnText, 0.7f, true);
            uITextPanel2->Width.Set(-10, 0.5f);
            uITextPanel2->Height.Set(50, 0);
            uITextPanel2->VAlign = 1;
            uITextPanel2->Top.Set(-45, 0);
            uITextPanel2->OnMouseOver += gcnew Terraria::UI::UIElement::MouseEvent(this, &CustomModInfoPage::_OnMouseOver);
            uITextPanel2->OnMouseOut += gcnew Terraria::UI::UIElement::MouseEvent(this, &CustomModInfoPage::_OnMouseOut);
            uITextPanel2->OnLeftClick += gcnew Terraria::UI::UIElement::MouseEvent(this, &CustomModInfoPage::_OnExitButton);
            uITextPanel2->SetSnapPoint("Back", 0, Vector2(0.5f), Vector2::Zero);
            uIElement->Append(uITextPanel2);
            _BackButton = uITextPanel;
            UITextPanel<String^>^ uITextPanel3 = gcnew UITextPanel<String^>(UpleadModText, 0.7f, true);
            uITextPanel3->CopyStyle(uITextPanel2);
            uITextPanel3->HAlign = 1;
            uITextPanel3->OnMouseOver += gcnew Terraria::UI::UIElement::MouseEvent(this, &CustomModInfoPage::_OnMouseOver);
            uITextPanel3->OnMouseOut += gcnew Terraria::UI::UIElement::MouseEvent(this, &CustomModInfoPage::_OnMouseOut);
            uITextPanel3->OnLeftClick += gcnew Terraria::UI::UIElement::MouseEvent(this, &CustomModInfoPage::_OnOtherButton);
            uIElement->Append(uITextPanel3);
            uITextPanel2->SetSnapPoint("New", 0, Vector2(0.5f), Vector2::Zero);
            _NewButton = uITextPanel3;

            _TextElement = gcnew Terraria::GameContent::UI::Elements::UIText(MainText, 1, false);
            _TextElement->IsWrapped = true;
            _TextElement->HAlign = 0;
            _TextElement->Top.Set(20, 0);
            _TextElement->Width.Set(0, 1);
            uIElement->Append(_TextElement);

            Append(uIElement);

        }


        ModListItem::ModListItem(Mod^ mod) {
            modRefrence = mod;
            BorderColor = Color(89, 116, 213) * 0.7f;
            String^ devider = "Images/UI/Divider";
            String^ innerBg = "Images/UI/InnerPanelBackground";
            String^ playerBg = "Images/UI/PlayerBackground";
            String^ infoIcon = "Images/UI/CharCreation/CharInfo";
            String^ openFile = "Images/UI/Camera_6";
            _dividerTexture = Terraria::Main::Assets->Request<Texture2D^>(devider, (AssetRequestMode)1);
            _innerPanelTexture = Terraria::Main::Assets->Request<Texture2D^>(innerBg, (AssetRequestMode)1);
            auto imgPath = mod->ModPath->Substring(0, mod->ModPath->LastIndexOf(".") + 1) + "png";
            if (System::IO::File::Exists(imgPath)) {
                System::IO::FileStream^ fileStream = gcnew System::IO::FileStream(imgPath, System::IO::FileMode::Open);
                _ModImage = Texture2D::FromStream(Terraria::Main::instance->GraphicsDevice, fileStream);
                fileStream->Close();
            }
            else _ModImage = Terraria::Main::Assets->Request<Texture2D^>(playerBg, (AssetRequestMode)1)->Value;
            _infoIcon = Terraria::Main::Assets->Request<Texture2D^>(infoIcon, (AssetRequestMode)1);
            _openFileIcon = Terraria::Main::Assets->Request<Texture2D^>(openFile, (AssetRequestMode)1);
            Height.Set(96, 0);
            Width.Set(0, 1);
            SetPadding(6);
            _playerPanel = gcnew UIElement();
            _playerPanel->Width.Set(59, 0);
            _playerPanel->Height.Set(58, 0);
            _playerPanel->Left.Set(4, 0);

            _OpenFileButton = gcnew UIImageButton(_openFileIcon);
            _OpenFileButton->VAlign = 1;
            _OpenFileButton->HAlign = 1;
            _OpenFileButton->Left.Set(2, 0);
            _OpenFileButton->Top.Set(4, 0);
            _OpenFileButton->OnLeftClick += gcnew Terraria::UI::UIElement::MouseEvent(this, &ModListItem::OnClickOpenMod);

            _OpenPageInfoButton = gcnew UIImageButton(_infoIcon);
            _OpenPageInfoButton->VAlign = 1;
            _OpenPageInfoButton->HAlign = 1;
            _OpenPageInfoButton->Left.Set(-30, 0);
            _OpenPageInfoButton->Top.Set(4, 0);
            _OpenPageInfoButton->OnLeftClick += gcnew Terraria::UI::UIElement::MouseEvent(this, &ModListItem::OnClickModInfo);

            Append(_OpenPageInfoButton);
            Append(_OpenFileButton);
            Append(_playerPanel);
        }
        void ModListItem::DrawSelf(SpriteBatch^ sb) {
            UIPanel::DrawSelf(sb);

            auto rect = GetDimensions();
            auto white = Color(255, 255, 255);
            auto srcRecNull = Nullable<Microsoft::Xna::Framework::Rectangle>();

            // draw author
            auto modNamePos = Vector2(rect.X + rect.Height + 4, rect.Y + 8);
            Terraria::Utils::DrawBorderString(sb, modRefrence->ModName, modNamePos, white, 1, 0, 0, -1);
            // draw plr background
            int plrBGPosOff = (rect.Height - _ModImage->Height) / 2;
            auto plrBGPos = Vector2(rect.X + plrBGPosOff, rect.Y + plrBGPosOff);
            sb->Draw(_ModImage, plrBGPos, srcRecNull, white, 0, Vector2::Zero, Vector2(1, 1), SpriteEffects::None, 0);
            // draw seperator
            auto sepPos = Vector2(rect.X + rect.Height, rect.Y + 30);
            sb->Draw(_dividerTexture->Value, sepPos, srcRecNull, white, 0, Vector2::Zero, Vector2((rect.Width - rect.Height) / 8, 1), SpriteEffects::None, 0);
            // draw pannels
            auto pnlWidth = ((rect.Width - rect.Height) / 2) - 10;
            auto pnlPos1 = Vector2(rect.X + rect.Height + 4, rect.Y + 36);
            DrawPanel(sb, pnlPos1, pnlWidth);
            Terraria::Utils::DrawBorderString(sb, modRefrence->ModAuthor, pnlPos1 + Vector2(4, 4), white, 1, 0, 0, -1);
            // draw pannel
            auto pnlPos2 = pnlPos1 + Vector2(pnlWidth + 10, 0);
            DrawPanel(sb, pnlPos2, pnlWidth);
            Terraria::Utils::DrawBorderString(sb, modRefrence->ModVersion, pnlPos2 + Vector2(pnlWidth - 4, 4), white, 1, 1, 0, -1);
            //// draw infoBtn
            //auto infoBtnPos = Vector2(rect.X + rect.Width - 2, rect.Y + rect.Width - 2);
            //sb->Draw(_openFileIcon->Value, sepPos, srcRecNull, white, 0, Vector2::Zero, Vector2((rect.Width - rect.Height) / 8, 1), SpriteEffects::None, 0);

        }

    }
}