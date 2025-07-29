

#include "pch.h"

#include "DefaultMod.h"
#include "Mod.h"
#include "ModHelper.h"
#include "ModLoader.h"

#include "CommonNamespaces.h"

namespace cModLoader {

    ModComponents::Mod^ ModLoader::GetMod(int index) {
        if (index >= modList->Count) return nullptr;
        return System::Linq::Enumerable::ElementAt(modList, index);
    }
    bool ModLoader::CheckDllPaths(System::String^ modDllPath) {
        for (int i = 0; i < modList->Count; i++)
            if (GetMod(i)->GetFullFilePath() == modDllPath)
                return false;
        return true;
    }
    ModComponents::ModListItem^ ModLoader::GetModUIElement(int index) {
        auto mod = GetMod(index);
        return mod == nullptr ? nullptr : mod->GetModListItem();
    }

    void ModLoader::LoadMod(ModComponents::Mod^ mod) {
        if (!upleadingMod) {
            System::Windows::Forms::MessageBox::Show("The mod '" + mod->ModName + "' was loaded ouside of the InjectTerrariaMod function.\nThe mod will not be loaded.\nPlease only load mods within this function.", "Error", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
            return;
        }
        if (!mod->OnInitialize()) return;
        upleadModCount++;
        modList->Add(mod);
		mod->OnModLoad();
    }

    bool ModLoader::NativeLoadModDll(String^ modDllPath) {
        if (!CheckDllPaths(modDllPath)) { MessageBox::Show("Mod Already Loaded.", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error); return false; }
		char* nativeString = static_cast<char*>(Marshal::StringToHGlobalAnsi(modDllPath).ToPointer());
        HMODULE hDll = LoadLibraryA(nativeString);
        if (!hDll) { MessageBox::Show("Failed to load Dll.\nWin32Err: " + Marshal::GetLastWin32Error(), "Error", MessageBoxButtons::OK, MessageBoxIcon::Error); return false; }
        FARPROC func = GetProcAddress(hDll, (LPSTR)"InjectTerrariaMod");
        if (func == nullptr) {
            MessageBox::Show("No exported InjectTerrariaMod fucntion detected.", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
            if (!FreeLibrary(hDll))
                MessageBox::Show("Failed to free dll.\nWin32Err: " + Marshal::GetLastWin32Error(), "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
            return false;
        }
        IntPtr intPtrHandle(reinterpret_cast<void*>(hDll));
        loadedDlls->Add(modDllPath, intPtrHandle);
        int preModLen = modList->Count;
        void* (*InjectTerrariaMod)(int) = (void* (*)(int))func;
        void* unmanagedPtr = InjectTerrariaMod(-1);
        if (upleadModCount == 0) {
            MessageBox::Show("No Mods were loaded.\nIf this is not intensional ignore it, otherwise create a mod within\n the exported InjectTerrariaMod fucntion to load a mod.", "Succsess?", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return true;
        }
        for (int i = preModLen; i < modList->Count; i++) {
			auto mod = GetMod(i);
			mod->ModFilePath = modDllPath->Substring(0, modDllPath->LastIndexOf("\\") + 1);
			mod->ModFileName = modDllPath->Substring(modDllPath->LastIndexOf("\\") + 1);
        }
        MessageBox::Show("All Mods Upleaded!", "Mods Upleaded", MessageBoxButtons::OK, MessageBoxIcon::Information);
        return true;
    }


    void ModLoader::Init() {
        Console::WriteLine("Dll inject!");
        cModLoader::CustomScreenRenderer::Init();

        Terraria::Main::versionNumber += " (cModLoader " + cModLoader::DefaultMod::version + ")";

        ModLoader::upleadingMod = true;
        auto defaultMod = gcnew cModLoader::DefaultMod();
        ModLoader::upleadingMod = false;

        auto modulesColection = System::Diagnostics::Process::GetCurrentProcess()->Modules;
        array<System::Diagnostics::ProcessModule^>^ modules = gcnew array<System::Diagnostics::ProcessModule^>(modulesColection->Count);
        modulesColection->CopyTo(modules, 0);
        for (int i = 0; i < modules->Length; i++) {
            auto module = modules[i];
            if (module->FileName->EndsWith("cModLoader.dll")) {
                cModLoader::ModLoader::dllPath = module->FileName;
                defaultMod->ModFilePath = module->FileName->Substring(0, module->FileName->LastIndexOf("\\") + 1);
                defaultMod->ModFileName = module->FileName->Substring(module->FileName->LastIndexOf("\\") + 1);
                break;
            }
        }

        Terraria::Main::OnPostDraw += gcnew System::Action<Microsoft::Xna::Framework::GameTime^>(&OnPostDraw);
        Terraria::Main::OnPreDraw += gcnew System::Action<Microsoft::Xna::Framework::GameTime^>(&OnPreDraw);

    }
    void ModLoader::OnPreDraw(Microsoft::Xna::Framework::GameTime^ obj) {
        int modCount = ModLoader::GetModCount;
        for (int i = 0; i < modCount; i++) {
            auto mod = ModLoader::GetMod(i);
            mod->OnPreDraw(obj);
        }
    }
    void ModLoader::OnPostDraw(Microsoft::Xna::Framework::GameTime^ obj) {
        static bool wasInWorld = false;
        if (Terraria::Main::menuMode == 0 && !ModHelper::isInWorld) {
            Terraria::Main::spriteBatch->Begin();
            Terraria::Main::MenuUI->MousePosition = Vector2(Terraria::Main::mouseX, Terraria::Main::mouseY);
            CustomScreenRenderer::Draw(Terraria::Main::spriteBatch);
            Terraria::Main::spriteBatch->End();
        }
        int modCount = ModLoader::GetModCount;

        bool isInWorld = ModHelper::isInWorld;
        bool LeftWorld = !isInWorld && wasInWorld;
        bool EnterWorld = isInWorld && !wasInWorld;
        wasInWorld = ModHelper::isInWorld;

        for (int i = 0; i < modCount; i++) {
            auto mod = ModLoader::GetMod(i);
            mod->OnPostDraw(obj);
			if (isInWorld) {
				mod->OnWorldPostDraw();
				AddModInterface(mod);
			}
            else mod->OnMenuPostDraw();
            if (LeftWorld) mod->OnLeaveWorld();
            if (EnterWorld) mod->OnEnterWorld();
        }
    }

	void ModLoader::AddModInterface(cModLoader::ModComponents::Mod^ mod) {
		if (mod->interfaceLayer == nullptr || mod->needAddInterface == false) return;
		bool _needToSetupDrawInterfaceLayers = (bool)(Terraria::Main::typeid)->GetField("_needToSetupDrawInterfaceLayers", System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::NonPublic)->GetValue(Terraria::Main::instance);
		if (!_needToSetupDrawInterfaceLayers) {
			auto list = (System::Collections::Generic::List<GameInterfaceLayer^>^)(Terraria::Main::typeid)->GetField("_gameInterfaceLayers", System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::NonPublic)->GetValue(Terraria::Main::instance);
			list->Insert(mod->interfaceLayerIndex, mod->interfaceLayer);
		}
		mod->needAddInterface = false;
	}
	void ModLoader::RemoveModInterface(cModLoader::ModComponents::Mod^ mod) {
		if (mod->interfaceLayer == nullptr) return;
		bool _needToSetupDrawInterfaceLayers = (bool)(Terraria::Main::typeid)->GetField("_needToSetupDrawInterfaceLayers", System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::NonPublic)->GetValue(Terraria::Main::instance);
		if (!_needToSetupDrawInterfaceLayers) {
			auto list = (System::Collections::Generic::List<GameInterfaceLayer^>^)(Terraria::Main::typeid)->GetField("_gameInterfaceLayers", System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::NonPublic)->GetValue(Terraria::Main::instance);
			list->Remove(mod->interfaceLayer);
		}
		mod->needAddInterface = true;
	}
}
