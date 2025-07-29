#pragma once
#include "Mod.h"
#include "ModListItem.h"

namespace cModLoader {
    namespace ModComponents {
        ref class ModListItem;
        ref class Mod;
    }
    
    ref class ModLoader {
    private:
    public:
        static System::Collections::Generic::List<cModLoader::ModComponents::Mod^>^ modList = gcnew System::Collections::Generic::List<cModLoader::ModComponents::Mod^>(1);
        static System::Collections::Generic::Dictionary<System::String^, System::IntPtr>^ loadedDlls = gcnew System::Collections::Generic::Dictionary<System::String^, System::IntPtr>(0);
        static property bool canUpleadMods { bool get() { return upleadingMod; } }
        static property int GetModCount { int get() { return modList->Count; } }
        //static void StartForceAllowUpleadMods() { upleadingMod = true; }
        //static void StopForceAllowUpleadMods() { upleadingMod = false; }
    internal:
        static ModComponents::Mod^ GetMod(int index);
        static ModComponents::ModListItem^ GetModUIElement(int index);
        static bool CheckDllPaths(System::String^ modDllPath);

        static void LoadMod(ModComponents::Mod^ mod);
        //static void UnloadMod(ModComponents::Mod^ mod);

        static bool NativeLoadModDll(System::String^ modDllPath);

        static void Init();
        static void OnPostDraw(Microsoft::Xna::Framework::GameTime^ obj);
        static void OnPreDraw(Microsoft::Xna::Framework::GameTime^ obj);
        //static void ReloadMods();

		static void AddModInterface(cModLoader::ModComponents::Mod^ mod);
		static void RemoveModInterface(cModLoader::ModComponents::Mod^ mod);

        static System::String^ dllPath = "";
        static bool upleadingMod = false;
        static int upleadModCount = 0;
    };
}
