#pragma once
#include "pch.h"
#include "ModInfoPage.h"
#include "ModListItem.h"
#include "ModLoader.h"

using namespace System;

namespace cModLoader {
	namespace ModComponents {
		Mod::Mod() { 
			ModLoader::LoadMod(this); 
		}
		ModInfoPage^ Mod::GetModInfoPage() { return gcnew cModLoader::ModComponents::ModInfoPage(this); }
		ModListItem^ Mod::GetModListItem() { return gcnew cModLoader::ModComponents::ModListItem(this); }
		void Mod::CreateGameInterfaceLayer(System::String^ layerName, int layerIndex, Terraria::UI::InterfaceScaleType UIscaleType) {
			interfaceLayer = gcnew Terraria::UI::LegacyGameInterfaceLayer(ModName + ":" + layerName, gcnew GameInterfaceDrawMethod(this, &Mod::GameInterfaceDraw), UIscaleType);
			interfaceLayerIndex = (layerIndex == -1 ? 37 : layerIndex);
		}
		bool Mod::GameInterfaceDraw() { OnGameInterfaceDraw(Terraria::Main::spriteBatch); return true; }

	}
}
