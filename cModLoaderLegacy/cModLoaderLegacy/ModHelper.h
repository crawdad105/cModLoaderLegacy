#pragma once
#include "pch.h"

#include "CommonNamespaces.h"

namespace cModLoader {

    public ref class ModHelper {
    public:
        static property bool isInWorld {
            bool get() { return !Terraria::Main::gameMenu; }
        }
        static property Terraria::Player^ userPlayer {
            Terraria::Player^ get() { return Terraria::Main::player[Terraria::Main::myPlayer]; }
        }
        static property array<Terraria::Item^>^ playerInventory {
            array<Terraria::Item^>^ get() { return userPlayer->inventory; }
        }
        static property array<Terraria::NPC^>^ NPCs {
            array<Terraria::NPC^>^ get() { return Terraria::Main::npc; }
        }
        static property array<Terraria::Player^>^ Players {
            array<Terraria::Player^>^ get() { return Terraria::Main::player; }
        }
        static property array<Terraria::Projectile^>^ Projectiles {
            array<Terraria::Projectile^>^ get() { return Terraria::Main::projectile; }
        }
        static Microsoft::Xna::Framework::Graphics::Texture2D^ GetTexture(System::String^ string) {
            return Terraria::Main::Assets->Request<Microsoft::Xna::Framework::Graphics::Texture2D^>(string, (ReLogic::Content::AssetRequestMode)1)->Value;
        }

        static property bool isLeftMouseDown {
            bool get() { return Terraria::GameInput::PlayerInput::MouseInfo.LeftButton == Microsoft::Xna::Framework::Input::ButtonState::Pressed; }
        }
        static property bool isLeftMouseUp {
            bool get() { return Terraria::GameInput::PlayerInput::MouseInfo.LeftButton == Microsoft::Xna::Framework::Input::ButtonState::Released; }
        }
        static property bool wasLeftMouseDown {
            bool get() { return Terraria::GameInput::PlayerInput::MouseInfoOld.LeftButton == Microsoft::Xna::Framework::Input::ButtonState::Pressed; }
        }
        static property bool wasLeftMouseUp {
            bool get() { return Terraria::GameInput::PlayerInput::MouseInfoOld.LeftButton == Microsoft::Xna::Framework::Input::ButtonState::Released; }
        }
        static property bool wasLeftMousePressDown {
            bool get() { return isLeftMouseDown && wasLeftMouseUp; }
        }
        static property bool wasLeftMousePressUp {
            bool get() { return isLeftMouseUp && wasLeftMouseDown; }
        }

        static property bool isRightMouseDown {
            bool get() { return Terraria::GameInput::PlayerInput::MouseInfo.RightButton == Microsoft::Xna::Framework::Input::ButtonState::Pressed; }
        }
        static property bool isRightMouseUp {
            bool get() { return Terraria::GameInput::PlayerInput::MouseInfo.RightButton == Microsoft::Xna::Framework::Input::ButtonState::Released; }
        }
        static property bool wasRightMouseDown {
            bool get() { return Terraria::GameInput::PlayerInput::MouseInfoOld.RightButton == Microsoft::Xna::Framework::Input::ButtonState::Pressed; }
        }
        static property bool wasRightMouseUp {
            bool get() { return Terraria::GameInput::PlayerInput::MouseInfoOld.RightButton == Microsoft::Xna::Framework::Input::ButtonState::Released; }
        }
        static property bool wasRightMousePressDown {
            bool get() { return isRightMouseDown && wasRightMouseUp; }
        }
        static property bool wasRightMousePressUp {
            bool get() { return isRightMouseUp && wasRightMouseDown; }
        }

    };

}