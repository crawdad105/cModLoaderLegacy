#include "pch.h"
#include "DefaultMod.h"
#include "ModLoader.h"
#include "CustomScreenRenderer.h"

#include "CommonNamespaces.h"

namespace cModLoader {

    void CustomScreenRenderer::_OnExitButton(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
        Terraria::Audio::SoundEngine::PlaySound(11, -1, -1, 1, 1, 0);
        Terraria::Main::menuMode = 0;
        Terraria::Main::MenuUI->SetState(nullptr);
    }
    void CustomScreenRenderer::_OnLoadDllButton(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
        auto pressedKeys = Terraria::GameInput::PlayerInput::GetPressedKeys();

        System::Windows::Forms::OpenFileDialog^ openFileDialog = gcnew System::Windows::Forms::OpenFileDialog();
        openFileDialog->Filter = "DLL Files (*.dll)|*.dll|All Files (*.*)|*.*";
        openFileDialog->Title = "Select a DLL File";
        if (openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            System::String^ selectedFilePath = openFileDialog->FileName;
            cModLoader::ModLoader::upleadingMod = true;
            cModLoader::ModLoader::upleadModCount = 0;
            cModLoader::ModLoader::NativeLoadModDll(selectedFilePath);
            cModLoader::ModLoader::upleadingMod = false;
        }
        else System::Windows::Forms::MessageBox::Show("No file selected.", "Warning", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Warning);
        UpdateList();
    }
    void CustomScreenRenderer::_OnMouseOver(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
        Terraria::Audio::SoundEngine::PlaySound(12, -1, -1, 1, 1, 0);
        ((Terraria::GameContent::UI::Elements::UIPanel^)evt->Target)->BackgroundColor = Microsoft::Xna::Framework::Color(73, 94, 171);
        ((Terraria::GameContent::UI::Elements::UIPanel^)evt->Target)->BorderColor = Terraria::ID::Colors::FancyUIFatButtonMouseOver;
    }
    void CustomScreenRenderer::_OnMouseOut(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
        ((Terraria::GameContent::UI::Elements::UIPanel^)evt->Target)->BackgroundColor = Microsoft::Xna::Framework::Color(63, 82, 151) * 0.7f;
        ((Terraria::GameContent::UI::Elements::UIPanel^)evt->Target)->BorderColor = Microsoft::Xna::Framework::Color(0, 0, 0);
    }
    
    void CustomScreenRenderer::UpdateList() {
        modListContainer->Clear();
        int count = ModLoader::GetModCount;
        for (int i = 0; i < count; i++) {
            auto elm = ModLoader::GetModUIElement(i);
            modListContainer->Add(elm);
        }

        if (_isScrollbarAttached && !modListScrollBar->CanScroll) {
            modMenuContainer->RemoveChild(modListScrollBar);
            _isScrollbarAttached = false;
            modListContainer->Width.Set(0, 1);
        }
        else if (!_isScrollbarAttached && modListScrollBar->CanScroll) {
            modMenuContainer->Append(modListScrollBar);
            _isScrollbarAttached = true;
            modListContainer->Width.Set(-25, 1);
        }

        modListContainer->Recalculate();
    }
    void CustomScreenRenderer::Init() {

        using namespace Terraria::UI;
        using namespace Terraria::GameContent::UI;
        using namespace Terraria::GameContent::UI::Elements;
        using namespace Microsoft::Xna::Framework;

        customMenuUI = gcnew Terraria::UI::UIState(); {
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
            uIElement->Append(uIPanel);
            modListContainer = gcnew UIList();
            modListContainer->Width.Set(0, 1);
            modListContainer->Height.Set(0, 1);
            modListContainer->ListPadding = 5;
            uIPanel->Append(modListContainer);
            modListScrollBar = gcnew UIScrollbar();
            modListScrollBar->SetView(100, 1000);
            modListScrollBar->Height.Set(0, 1);
            modListScrollBar->HAlign = 1;
            modListContainer->SetScrollbar(modListScrollBar);
            UITextPanel<System::String^>^ uITextPanel = gcnew UITextPanel<System::String^>(ModMenu_TitleText, 0.8f, true);
            uITextPanel->HAlign = 0.5f;
            uITextPanel->Top.Set(-40, 0);
            uITextPanel->SetPadding(15);
            uITextPanel->BackgroundColor = Color(73, 94, 171);
            uIElement->Append(uITextPanel);
            UITextPanel<System::String^>^ uITextPanel2 = gcnew UITextPanel<System::String^>(ModMenu_BackBtnText, 0.7f, true);
            uITextPanel2->Width.Set(-10, 0.5f);
            uITextPanel2->Height.Set(50, 0);
            uITextPanel2->VAlign = 1;
            uITextPanel2->Top.Set(-45, 0);
            uITextPanel2->OnMouseOver += gcnew UIElement::MouseEvent(&_OnMouseOver);
            uITextPanel2->OnMouseOut += gcnew UIElement::MouseEvent(&_OnMouseOut);
            uITextPanel2->OnLeftClick += gcnew UIElement::MouseEvent(&_OnExitButton);
            uITextPanel2->SetSnapPoint("Back", 0, Vector2(0.5f), Vector2::Zero);
            uIElement->Append(uITextPanel2);
            UITextPanel<System::String^>^ uITextPanel3 = gcnew UITextPanel<System::String^>(ModMenu_UpleadModText, 0.7f, true);
            uITextPanel3->CopyStyle(uITextPanel2);
            uITextPanel3->HAlign = 1;
            uITextPanel3->OnMouseOver += gcnew UIElement::MouseEvent(&_OnMouseOver);
            uITextPanel3->OnMouseOut += gcnew UIElement::MouseEvent(&_OnMouseOut);
            uITextPanel3->OnLeftClick += gcnew UIElement::MouseEvent(&_OnLoadDllButton);
            uIElement->Append(uITextPanel3);
            uITextPanel2->SetSnapPoint("New", 0, Vector2(0.5f), Vector2::Zero);
            customMenuUI->Append(uIElement);
        }

        Button = gcnew UITextPanel<System::String^>(OpenModMenuButtonText, 1, false);
        Button->Left.Set(10, 0);
        Button->Top.Set(10, 0);

    }
    void CustomScreenRenderer::OnCustomMenuOpen() {
        Terraria::Audio::SoundEngine::PlaySound(10, -1, -1, 1, 1, 0);
        Terraria::Main::menuMode = 888;
        Terraria::Main::MenuUI->SetState(customMenuUI);
        UpdateList();
    }
    void CustomScreenRenderer::Draw(Microsoft::Xna::Framework::Graphics::SpriteBatch^ batch) {
        if (Button->ContainsPoint(Terraria::Main::MenuUI->MousePosition)) {
            Button->TextColor = Microsoft::Xna::Framework::Color(253, 216, 53);
            if (
                Terraria::GameInput::PlayerInput::MouseInfo.LeftButton.ToString() == "Pressed" &&
                Terraria::GameInput::PlayerInput::MouseInfoOld.LeftButton.ToString() != "Pressed")
                OnCustomMenuOpen();
        }
        else Button->TextColor = Microsoft::Xna::Framework::Color(255, 255, 255);
        Button->Recalculate();
        Button->Draw(batch);
    }
}