#pragma once
#include "Mod.h"

namespace cModLoader {
    ref class DefaultMod : cModLoader::ModComponents::Mod {
    public:

        static System::String^ url = "https://www.crawdad105.com";
        static System::String^ version = "v1.0 (legacy)";
        static System::String^ author = "Crawdad105";
        static System::String^ name = "cModLoader";
        static System::String^ description =
            "This is the default built in mod for cModLoader." +
            "\nIt was easyer to create a mod then hard code functions into the code." +
            "\n" +
            "\n";

        cModLoader::ModComponents::ModInfoPage^ GetModInfoPage() override;
        cModLoader::ModComponents::ModListItem^ GetModListItem() override;

        bool OnInitialize() override {
            ModName = name;
            ModAuthor = author;
            ModDescription = description;
            ModUrl = url;
            ModVersion = version;
            return true;
        }

    };
}