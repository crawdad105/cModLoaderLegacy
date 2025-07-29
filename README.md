# cModLoader (Crawdad's mod loader) (Legacy)

cModLoader is a mod loader for vanilla terraria allowing the user to run custom code in vanilla terraria via a dll file. This is the legacy version using dll injection and requires c++. A newer version that does not require dll injection (for loading base code) nor c++ is in the works and should make it easier to modify existing Terraria code using Harmony or something. This does not modify any of terraria's code so terraria runs normaly and it is not a cracked/modifed version of the exe. 

## How it works

The user will download the initializer exe along with the cModLoader.dll file. After running the initializer it will find the terraria directory and renames the Terraria exe to RealTerraria.exe and then copies itself and changes the name to Terraria.exe, this then allows cModLoader to run from steam. After installing the user can open terraria like normal and some code will run that launches terraria and injects a dll that inserts itself into Terraria's main thread. When terraria opens a new button in the top left corner of the screen is visible clicking on this will open a new mod menu where the user can upload mods. 
(This does **NOT** run tModLoader mods)

## How to run a mod

When uploading a mod the user will be prompted to select a dll file, assuming this is a valid mod file it will run whatever code is within the dll. Currently there are 3 official mod, thoes being an auto fisher, velocity viewer (for the player) and one that adds more details like drops when hovering over enemies. The auto fisher is the middle child [predecessor to this one](https://crawdad105.itch.io/terraria-auto-fisher) this version on this repositor probably won't be updated but once I create the real cModLoader I will add multiple other example mods including a new working auto fisher which will be the auto fisher 2.0 mentioned on the itch.io page.

## Removing cModLoader

If you wish to remove cModLoader to play vanila terraria without modifications you can run the `cModLoaderRemoverLegacy.exe` file located in the terraria directory. Unfortunitaly with windoes defenders it will probably flag it as a virus since runs code seperate from the program to remove other programs and itself. If it ends up failing due to that, you can reinstall terraria to fix it easily. 

## Unloading mods

It is impossible. Although there are many ways to unload dlls from a program, there is no way I could find to unload these dlls while being able to reload them, so live updating like tModLaoder is not possible. (tModLoader was made using a different and newer .net framework which allows for this, but it is not possible using .net 4.X like terraria uses)

## How to create a mod

A valid mod dll consists of a dll file with an exported function called "InjectTerrariaMod", this is the functions that will be ran allowing custom code to be ran. The dll needs to be 32 bit. Although not technically required it makes modding a lot easier using c++ with common language runtime, this allows for running terraria code like `Terraria::Main`. In the "InjectTerrariaMod" functions you should create an instance of a custom class that inherits the Mod class from `cModLoader::ModComponents::Mod` this adds some functions that allow you to do things like run code in the main thread and add UI elements, all 3 example mods do this. Initializing an instance of the Mod class outside of the InjectTerrariaMod function will give you an error, this is so a mod isn't loaded at a bad time (feel free to fork cModLoaderLegacy to remove this, its just to make sure the mods load properly). Unfortunately this is not tModLoader meaning without modifying the vanilla code its not easy to run code directly before or after the actual game update, currently only a post or pre draw update is provided. Feel free to use the provided example mods for the code to develop your own mod.  

If using c++/clr yould need to add some references to your project so the mod can compile and run, those include:
* Microsoft.Xna.Framework
* Microsoft.Xna.Framework.Game
* Microsoft.Xna.Framework.Graphics
* ReLogic (this can be extracted using IlSpy or something or found within the tmod files)
* Terraria (use the RealTerraria.exe if cModLoaderLegacy is already installed)

(the framweork dlls can be found at `C:\Windows\Microsoft.NET\assembly\GAC_32` (I don't know if this is always the case))

## When is the full version?

As stated before this is only a legacy version (despite it coming out before the full version). This version of cModLoader was made using c++ and requires c++ dll to run. Because c# is easyer to understand I do want the full version to run using c# and take c# and c++ dlls as mods. I want to make it partially compatible with tMod files but I am not sure if this will happen.

## Need help?

Join the official Crawdad105 discord server if you need help creating a mod, reading or wrighting code.
https://discord.com/invite/5NKSqxkckA

## Can I "borrow" the code?

Yes. feel free to fork, steal or copy any code here. Although you don't need to do this, I suggest marking mods made for this specific version of cModLoader as legacy, so when the real cModLoader comes out there isn't any confusion (however I will try making it compatible). And if you create a mod feel free to share it in the discord server and I might add a link to the mod here for anyone else (if its a virus you will be banned).
