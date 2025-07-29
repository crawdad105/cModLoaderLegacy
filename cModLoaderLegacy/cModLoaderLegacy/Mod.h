#pragma once
#include "ModInfoPage.h"
#include "ModListItem.h"
#include "ModLoader.h"

namespace cModLoader {
    namespace ModComponents {
        
        ref class ModInfoPage;
        ref class ModListItem;

        public ref class Mod {
        public:
            Mod();
            System::String^ ModName = "";
            System::String^ ModDescription = "";
            System::String^ ModAuthor = "";
            System::String^ ModVersion = "";
            System::String^ ModUrl = "";
            Microsoft::Xna::Framework::Graphics::Texture2D^ imageIcon;

            virtual cModLoader::ModComponents::ModInfoPage^ GetModInfoPage();
            virtual cModLoader::ModComponents::ModListItem^ GetModListItem();

            virtual bool OnInitialize() { return true; }

            virtual void OnPostDraw(Microsoft::Xna::Framework::GameTime^ gameTime) {}
            virtual void OnPreDraw(Microsoft::Xna::Framework::GameTime^ gameTime) {}
            virtual void OnWorldPostDraw() {}
            virtual void OnMenuPostDraw() {}
            virtual void OnEnterWorld() {}
            virtual void OnLeaveWorld() {}
            virtual void OnGameInterfaceDraw(Microsoft::Xna::Framework::Graphics::SpriteBatch^ spriteBatch) {}
            void CreateGameInterfaceLayer(System::String^ layerName, int layerIndex, Terraria::UI::InterfaceScaleType UIscaleType);
        
			virtual void OnModLoad() {}

        private:
            bool GameInterfaceDraw();

        internal:
            Terraria::UI::LegacyGameInterfaceLayer^ interfaceLayer = nullptr;
            int interfaceLayerIndex;
			bool needAddInterface = true;
			bool isModLoaded;
            System::String^ ModFilePath = "";
            System::String^ ModFileName = "";
            System::String^ GetFullFilePath() { return ModFilePath + ModFileName; }
        };
    }
}