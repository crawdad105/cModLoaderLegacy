// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

using namespace System;
using namespace Microsoft::Xna::Framework;

ref class CustomMod : cModLoader::ModComponents::Mod {

public:
    CustomMod(System::String^ name, System::String^ desc, System::String^ author, System::String^ version) {
        CreateGameInterfaceLayer("Custom Layer: Draw Velocity", -1, Terraria::UI::InterfaceScaleType::Game);

        ModName = name;
        ModAuthor = author;
        ModDescription = desc;
        ModVersion = version;

    }

    void OnGameInterfaceDraw(Microsoft::Xna::Framework::Graphics::SpriteBatch^ sb) override {

        for (int i = 0; i < Terraria::Main::maxNPCs; i++) {
            Terraria::NPC^ npc = Terraria::Main::npc[i];
            // dont check inactive npcs
            if (npc->active == false) continue;

            int maxDist = 25;

            int w = 2;
            // check if mouse is near the npc

            Microsoft::Xna::Framework::Rectangle value = Microsoft::Xna::Framework::Rectangle(npc->TopLeft.X, npc->TopLeft.Y, npc->width, npc->height);

            if (value.Contains(Terraria::Main::MouseWorld.X, Terraria::Main::MouseWorld.Y)) {
                Terraria::Utils::DrawRectangle(sb, npc->TopLeft, npc->BottomRight, Color::Green, Color::Green, w);

                // get colour present from red to green for current NPC health
                float h = (float)npc->life / (float)npc->lifeMax;
                int r = 0;
                int g = 0;
                if (h > 0.5) {
                    r = (1 - h) * 2 * 255;
                    g = 255;
                }
                else {
                    r = 255;
                    g = h * 2 * 255;
                }
                String^ colour = r.ToString("X2") + g.ToString("X2") + "00";

                String^ text = npc->GivenOrTypeName + " [c/646464:(id: " + npc->type + ", index: " + i + ")]: [c/" + colour + ":" + npc->life + "] / [c/00FF00:" + npc->lifeMax + "]";

                /* optional: print AI stats (normal players won't need this)
                int maxAI = npc->maxAI;
                text += "\nai:";
                for (int n = 0; n < maxAI; n++) {
                    text += " " + npc->ai[n];
                    if (n < maxAI - 1) text += ", ";
                }
                text += " | LocalAI:";
                for (int n = 0; n < maxAI; n++) {
                    text += " " + npc->localAI[n];
                    if (n < maxAI - 1) text += ", ";
                }
                */

                auto entry = Terraria::Main::BestiaryDB->FindEntryByNPCID(npc->type);
                for (int j = 0; j < entry->Info->Count; j++) {
                    using namespace Terraria::GameContent::Bestiary;
                    IBestiaryInfoElement^ elm = entry->Info[j];
                    auto type = elm->GetType();
                    // check if bestiary info element is of type ItemDropBestiaryInfoElement 
                    if (type == ItemDropBestiaryInfoElement::typeid) {
                        bool shouldDisplay = true;
                        auto _droprateInfo = type->GetField("_droprateInfo", System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance);
                        Terraria::GameContent::ItemDropRules::DropRateInfo^ dropInfo = (Terraria::GameContent::ItemDropRules::DropRateInfo^)_droprateInfo->GetValue(elm);
                        // check if condition exist
                        if (dropInfo->conditions != nullptr) {
                            // if condition dose exist check each index and call CanShowItemDropInUI to check if it should display (this is so difficulty exclusive drops don't display unless in the proper difficulty)
                            for (int k = 0; k < dropInfo->conditions->Count; k++) {
                                shouldDisplay = dropInfo->conditions[k]->CanShowItemDropInUI();
                                // check if it returns false in which case it will not be displayed
                                if (shouldDisplay == false)
                                    break;
                            }
                        }
                        // display only if shouldDisplay is true
                        if (shouldDisplay) {
                            text += "\n[i:" + dropInfo->itemId + "] (" + dropInfo->stackMin + "-" + dropInfo->stackMax + ") 1 in " + (1 / dropInfo->dropRate) + " (" + (dropInfo->dropRate * 100) + "%)";
                        }
                    }
                    // check if bestiary info element is of type NPCKillCounterInfoElement 
                    else if (type == NPCKillCounterInfoElement::typeid) {
                        text += "\nKills: " + Terraria::Main::BestiaryTracker->Kills->GetKillCount(npc);
                    }
                    // check if bestiary info element is of type NPCStatsReportInfoElement 
                    else if (type == NPCStatsReportInfoElement::typeid) {
                        NPCStatsReportInfoElement^ _elm = ((NPCStatsReportInfoElement^)elm);
                        int value = (int)_elm->MonetaryValue;
                        String^ platinum = "[i:74]" + Terraria::Utils::Clamp(value / 1000000, 0, 999).ToString() + ", ";
                        String^ gold = "[i:73]" + Terraria::Utils::Clamp(value % 1000000 / 10000, 0, 99).ToString() + ", ";
                        String^ silver = "[i:72]" + Terraria::Utils::Clamp(value % 10000 / 100, 0, 99).ToString() + ", ";
                        String^ copper = "[i:71]" + Terraria::Utils::Clamp(value % 100 / 1, 0, 99).ToString();
                        if (value / 1000000 < 1) platinum = "";
                        if (value / 10000 < 1) gold = "";
                        if (value / 100 < 1) silver = "";
                        if (value < 1) continue; // continue because it would end up adding nothing
                        text += "\n" + platinum + gold + silver + copper;
                        text += "\nDamage: " + _elm->Damage + " | Defence: " + _elm->Defense;
                    }
                    else {
                        // if the bestiary info element type is something else dont print it
                        // text += "\n" + type;
                    }
                }

                // set mouse text to nothing to print other text, mouse text dose not print in colour
                Terraria::Main::instance->MouseTextHackZoom("", 0, 0, nullptr);
                Terraria::Main::mouseText = true;
                
                Vector2 pos = Vector2(Terraria::Main::mouseX + 30, Terraria::Main::mouseY + 30);
                // effectively overrides Main.MouseTextInner()
                Terraria::Utils::DrawBorderString(sb, text, pos, Color::White, 1, 0, 0, -1);

            }
            else {
                // dont print unnecessary boxes unless the player is hoving over npc
                // Terraria::Utils::DrawRectangle(sb, npc->TopLeft, npc->BottomRight, Color::Red, Color::Red, w);
            }
            
            // optionaly print text at npc (all the time)
            Vector2 tesxtPos = npc->BottomRight + (-Terraria::Main::screenPosition);
            String^ str = "";
            Terraria::Utils::DrawBorderString(sb, str, tesxtPos, Color::White, 1, 0, 0, -1);

        }

    }

};

extern "C" __declspec(dllexport) void* InjectTerrariaMod(int i) {

    // create mod
    auto mod = gcnew CustomMod(
        "NPC Details",
        "This mod made for cModLoader (legacy version) displays information about an npc when hovering the mouse over them.",
        "crawdad105",
        "v1.0"
    );

    return NULL;
}
