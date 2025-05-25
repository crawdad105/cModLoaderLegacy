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
    /* code to get average velocity (not used)
    const int max = 100;
    array<float>^ avgVolX = gcnew array<float>(max);
    array<float>^ avgVolY = gcnew array<float>(max);
    */
    void OnGameInterfaceDraw(Microsoft::Xna::Framework::Graphics::SpriteBatch^ sb) override {

        // get local player
        Terraria::Player^ p = Terraria::Main::player[Terraria::Main::myPlayer];
        int w = 2; // line width
        int mult = 10; // line size multiplier

        /* code to get average velocity (not used)

        float avgX = 0;
        float avgY = 0;
        for (int i = 1; i < max; i++)
        {
            avgVolX[i - 1] = avgVolX[i];
            avgVolY[i - 1] = avgVolY[i];
            avgX += avgVolX[i];
            avgY += avgVolY[i];
        }
        avgVolX[avgVolX->Length - 1] = p->velocity.X;
        avgX += p->velocity.X;
        avgX /= max;
        avgVolY[avgVolY->Length - 1] = p->velocity.Y;
        avgY += p->velocity.Y;
        avgY /= max;
        Vector2 avgVecVolX = Vector2(1, 0) * avgX;
        Vector2 avgVecVolY = Vector2(0, 1) * avgY;
        Vector2 avgVecVol = Vector2(avgX, avgY);

        */

        float volX = p->velocity.X;
        float volY = p->velocity.Y;
        Vector2 vecVolX = Vector2(1, 0) * volX;
        Vector2 vecVolY = Vector2(0, 1) * volY;
        Vector2 vecVol = p->velocity;

        // construct display string
        String^ str = "x: " + p->velocity.X + "\ny: " + p->velocity.Y + "\nVol : |" + vecVol.Length() + "|";

        // draw velocity lines
        Terraria::Utils::DrawLine(sb, p->Center, p->Center + vecVolX * mult, Color::Red, Color::Red, w);
        Terraria::Utils::DrawLine(sb, p->Center, p->Center + vecVolY * mult, Color::Blue, Color::Blue, w);
        Terraria::Utils::DrawLine(sb, p->Center, p->Center + vecVol * mult, Color::Green, Color::Green, w);

        // using (mult - Terraria::Main::screenPosition) gives an error
        Vector2 drawPosX = p->Center + vecVolX * mult + (-Terraria::Main::screenPosition);
        Vector2 drawPosY = p->Center + vecVolY * mult + (-Terraria::Main::screenPosition);
        Vector2 drawPos = p->Center + vecVol * mult + (-Terraria::Main::screenPosition);

        // draw velocity text
        Terraria::Utils::DrawBorderString(sb, "" + volX + ", " + (int)volX, drawPosX, Color::Red, 1, 0, 0, -1);
        Terraria::Utils::DrawBorderString(sb, "" + volY + ", " + (int)volY, drawPosY, Color::Blue, 1, 0, 0, -1);
        Terraria::Utils::DrawBorderString(sb, "" + vecVol.Length() + ", " + (int)vecVol.Length(), drawPos, Color::Cyan, 1, 0, 0, -1);

        // draw display text
        Terraria::Utils::DrawBorderString(sb, str, p->Center + (-Terraria::Main::screenPosition) + (-Vector2(0, 140)), Color::White, 1, 0, 0, -1);
	
	}

};

extern "C" __declspec(dllexport) void* InjectTerrariaMod(int i) {

    // create mod
	auto mod = gcnew CustomMod(
        "Display Player Velocity", 
        "This mod made for cModLoader (legacy version) draws custom screen elements to display arrows for velocity, this can be useful for testing velocity.", 
        "crawdad105", 
        "v1.0"
    );

	return NULL;
}
