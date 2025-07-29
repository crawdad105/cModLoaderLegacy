#include "pch.h"
#include "DefaultMod.h"

#include "CommonNamespaces.h"

namespace cModLoader {

    cModLoader::ModComponents::ModInfoPage^ DefaultMod::GetModInfoPage() {
        auto page = Mod::GetModInfoPage();
        page->_TitleElement->TextColor = Color(84, 255, 0);
        return page;
    }

    cModLoader::ModComponents::ModListItem^ DefaultMod::GetModListItem() {
        auto panel = Mod::GetModListItem();
        panel->_TitleElement->TextColor = Color(84, 255, 0);
        return panel;
    }

}
