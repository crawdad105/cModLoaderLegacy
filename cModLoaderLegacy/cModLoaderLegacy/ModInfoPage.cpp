#include "pch.h"
#include "ModInfoPage.h"
#include "CommonNamespaces.h"

namespace cModLoader {
    namespace ModComponents {
        
        void ModInfoPage::_OnMouseOver(UIMouseEvent^ evt, UIElement^ listeningElement) {
            SoundEngine::PlaySound(12, -1, -1, 1, 1, 0);
            ((UIPanel^)evt->Target)->BackgroundColor = Color(73, 94, 171);
            ((UIPanel^)evt->Target)->BorderColor = Terraria::ID::Colors::FancyUIFatButtonMouseOver;
        }
        void ModInfoPage::_OnMouseOut(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
            ((UIPanel^)evt->Target)->BackgroundColor = Color(63, 82, 151) * 0.7f;
            ((UIPanel^)evt->Target)->BorderColor = Color(0, 0, 0);
        }
        void ModInfoPage::_OnExitButton(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
            if (!OnExitButton()) return;
            Terraria::Audio::SoundEngine::PlaySound(11, -1, -1, 1, 1, 0);
            Terraria::Main::MenuUI->SetState(cModLoader::CustomScreenRenderer::customMenuUI);
        }
        void ModInfoPage::_OnOtherButton(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
            if (!OnOtherButton()) return;
            if (PageUrl != "") {
                if (PageUrl->StartsWith("http://") || PageUrl->StartsWith("https://"))
                    System::Diagnostics::Process::Start("explorer.exe", PageUrl);
                else System::Windows::Forms::MessageBox::Show("Invalid link.\n" + PageUrl + "\nURL must be in format 'https://www.example.com'.", "Inavlid URL", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
            }
        }
    
        ModInfoPage::ModInfoPage(Mod^ mod) { 
            TitleText = mod->ModName; 
            MainText = mod->ModDescription; 
            PageUrl = mod->ModUrl;

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
            _TitleElement = gcnew UITextPanel<String^>(TitleText, 0.8f, true);
            _TitleElement->HAlign = 0.5f;
            _TitleElement->Top.Set(-40, 0);
            _TitleElement->SetPadding(15);
            _TitleElement->BackgroundColor = Color(73, 94, 171);
            uIElement->Append(_TitleElement);
            _BackButton = gcnew UITextPanel<String^>(BackBtnText, 0.7f, true);
            _BackButton->Width.Set(-10, 0.5f);
            _BackButton->Height.Set(50, 0);
            _BackButton->VAlign = 1;
            _BackButton->Top.Set(-45, 0);
            _BackButton->OnMouseOver += gcnew Terraria::UI::UIElement::MouseEvent(this, &ModInfoPage::_OnMouseOver);
            _BackButton->OnMouseOut += gcnew Terraria::UI::UIElement::MouseEvent(this, &ModInfoPage::_OnMouseOut);
            _BackButton->OnLeftClick += gcnew Terraria::UI::UIElement::MouseEvent(this, &ModInfoPage::_OnExitButton);
            _BackButton->SetSnapPoint("Back", 0, Vector2(0.5f), Vector2::Zero);
            uIElement->Append(_BackButton);
            _NewButton = gcnew UITextPanel<String^>(UpleadModText, 0.7f, true);
            _NewButton->CopyStyle(_BackButton);
            _NewButton->HAlign = 1;
            _NewButton->OnMouseOver += gcnew Terraria::UI::UIElement::MouseEvent(this, &ModInfoPage::_OnMouseOver);
            _NewButton->OnMouseOut += gcnew Terraria::UI::UIElement::MouseEvent(this, &ModInfoPage::_OnMouseOut);
            _NewButton->OnLeftClick += gcnew Terraria::UI::UIElement::MouseEvent(this, &ModInfoPage::_OnOtherButton);
            uIElement->Append(_NewButton);
            _NewButton->SetSnapPoint("New", 0, Vector2(0.5f), Vector2::Zero);

            _TextElement = gcnew Terraria::GameContent::UI::Elements::UIText(MainText, 1, false);
            _TextElement->IsWrapped = true;
            _TextElement->HAlign = 0;
            _TextElement->Top.Set(20, 0);
            _TextElement->Width.Set(0, 1);
            uIElement->Append(_TextElement);

            Append(uIElement);
        
        }
 
    }
}