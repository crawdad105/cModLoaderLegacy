#include "pch.h"
#include "ModLoader.h"

extern "C" __declspec(dllexport) void HookMainMenuInterface() { cModLoader::ModLoader::Init(); }