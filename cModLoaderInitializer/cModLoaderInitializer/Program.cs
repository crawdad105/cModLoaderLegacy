using System.Diagnostics;
using System.Drawing;

namespace cModLoaderInitializerLegacy
{
    public class Program
    {

        public static string SteamPathEnding = @"\steamapps\common\Terraria\Terraria.exe";
        public static string RemoverPathEnding = @"\steamapps\common\Terraria\cModLoaderRemoverLegacy.exe";

        public static bool FindTerraria(out string terrariaPath) {
            terrariaPath = "";
            string defaultPath = $@":\Program Files (x86)\Steam{SteamPathEnding}";
            string movedPath = $@":\SteamLibrary{SteamPathEnding}";
            foreach (var leter in "ABCDEFGHJKLMNOPQRSTUVWXYZ".ToCharArray()) {
                if (File.Exists(leter + defaultPath)) terrariaPath = leter + defaultPath;
                if (File.Exists(leter + movedPath))   terrariaPath = leter + movedPath;
            }
            return terrariaPath == "" ? false : true;
        }

        public enum Status {
            Inistialize,
            RunTerraria,
            Remove,
            ReturnError,
        }
        public static Status GetStatus(out string exacutionPath) {
            exacutionPath = "";
            string? tempPath = Environment.ProcessPath;
            if (tempPath == null) {
                Print("Error: Faild to find self exacution path.", ConsoleColor.Red);
                Console.ReadKey();
                return Status.ReturnError;
            }
            exacutionPath = tempPath;
            if (exacutionPath.EndsWith(SteamPathEnding)) return Status.RunTerraria;
            else if (exacutionPath.EndsWith(RemoverPathEnding)) return Status.Remove;
            else return Status.Inistialize;
        }

        public static bool DoInistialize(string exacutionPath) {
            // check cmodloader file exist
            string cModLoaderPath = exacutionPath.Substring(0, exacutionPath.LastIndexOf("\\") + 1) + "cModLoaderLegacy.dll";
            if (!File.Exists(cModLoaderPath)) {
                Print("Error: Could not find 'cModLoaderLegacy.dll' within the same directory as this EXE.\nPleace make sure a 'cModLoaderLegacy.dll' file exist.", ConsoleColor.Red);
                Console.ReadKey();
                return false;
            }
            // check terraria exist
            if (!FindTerraria(out string terrariaPath)) {
                Print("Error: Could not find the Terraria file within the normal steam directories.\nMake sure Terraria is properly installed.", ConsoleColor.Red);
                Console.ReadKey();
                return false;
            }
            // check terraria is not running
            foreach (var proc in Process.GetProcessesByName("Terraria"))
                for (int i = 0; i < proc.Modules.Count; i++)
                    if (proc.Modules[i].FileName == terrariaPath) {
                        Print("Error: Terraria was found running.\nClose Terraria before running this EXE.", ConsoleColor.Red);
                        Console.ReadKey();
                        return false;
                    }
            // Do file stuff
            string RealTerrariaPath = terrariaPath.Substring(0, terrariaPath.LastIndexOf("\\") + 1) + "RealTerraria.exe";
            if (File.Exists(RealTerrariaPath)) {
                Print("Error: cModLoader is already Initialized.", ConsoleColor.Red);
                Console.ReadKey();
                return false;
            }
            string cModLoaderFolderPath = terrariaPath.Substring(0, terrariaPath.LastIndexOf("\\") + 1) + "cModLoaderLegacy\\";
            if (!Directory.Exists(cModLoaderFolderPath)) Directory.CreateDirectory(cModLoaderFolderPath);
            int num = 0;
            try {
                byte[]? arr = (byte[]?)Properties.Resources.ResourceManager.GetObject("cModLoader");
                if (arr == null)
                    Print("Error: Failed to retreave cModLoader icon.", ConsoleColor.Red);
                else {
                    File.Create(cModLoaderFolderPath + "icon.ico").Close();
                    File.WriteAllBytes(cModLoaderFolderPath + "icon.ico", arr);
                }
                num++;
                File.Copy(cModLoaderPath, cModLoaderFolderPath + "cModLoaderLegacy.dll");
                num++;
                File.Move(terrariaPath, RealTerrariaPath);
                num++;
                File.Copy(exacutionPath, terrariaPath.Substring(0, terrariaPath.LastIndexOf("\\") + 1) + "Terraria.exe");
                num++;
                File.Copy(exacutionPath, terrariaPath.Substring(0, terrariaPath.LastIndexOf("\\") + 1) + "cModLoaderRemoverLegacy.exe");
            } catch (Exception e) {
                if (num == 0)
                    Print("Error: Failed to create icon file.\n\n" + e.Source + "\n\n" + e.InnerException + "\n\n" + e.StackTrace, ConsoleColor.Red);
                else if (num == 1)
                    Print("Error: Could not move cModLoaderLegacy.dll file.", ConsoleColor.Red);
                else if (num == 2)
                    Print("Error: Failed to rename the original Terraria file.", ConsoleColor.Red);
                else if (num == 3 || num == 4)
                    Print("Error: Failed to copy terraria exacuter.", ConsoleColor.Red);
                else Print("Error: Unknown Error.\n(This message should not be possible to get)", ConsoleColor.Red);
                Console.ReadKey();
                return false;
            }

            return true;
        }
        public static bool DoRemove(string exacutionPath) {
            string cModLoaderInitPath = exacutionPath.Substring(0, exacutionPath.LastIndexOf("\\") + 1) + "Terraria.exe";
            string RealTerrariaPath = exacutionPath.Substring(0, exacutionPath.LastIndexOf("\\") + 1) + "RealTerraria.exe";
            if (!File.Exists(RealTerrariaPath)) {
                Print("Error: cModLoader was not Initialized.");
                Console.ReadKey();
                return false;
            }
            string cModLoaderFolderPath = exacutionPath.Substring(0, exacutionPath.LastIndexOf("\\") + 1) + "cModLoaderLegacy\\";
            if (Directory.Exists(cModLoaderFolderPath)) {
                try {
                    Directory.Delete(cModLoaderFolderPath, true);
                } catch (Exception) {
                    Print("Error: Could not remove cModLoader file directory.", ConsoleColor.Red);
                    Console.ReadKey();
                    return false;
                }
            }
            try {
                Thread.Sleep(500);
                File.Delete(cModLoaderInitPath);
            } catch (Exception) { }
            try {
                Thread.Sleep(500);
                File.Delete(cModLoaderInitPath);
            } catch (Exception) {
                Print("Error: Failed to remove cModLoaderInitializer which is the file Terraria.exe.", ConsoleColor.Red);
                Console.ReadKey();
                return false;
            }
            try {
                File.Move(RealTerrariaPath, cModLoaderInitPath);
            } catch (Exception) {
                Print("Error: Failed to rename ReadTerraria.exe to Terraria.exe\nTo fix this, do it yourself or reinstall terraria.", ConsoleColor.Red);
                Console.ReadKey();
                return false;
            }
            return true;
        }

        public static void Main(string[] args) {
            Status status = GetStatus(out string exacutionPath);
            if (status == Status.ReturnError) {
                Print("An unknown occured.", ConsoleColor.Red);
                Console.ReadKey();
                return;
            }
            else if (status == Status.Remove) {
                Print("Removing cModLoader...", ConsoleColor.Green);
                if (!DoRemove(exacutionPath)) return;
                Print("cModLoader removed!", ConsoleColor.Green);
                Print("You can now play normal terraria now.", ConsoleColor.Green);
                Print("(This program will self-destruct)");
                Print("Press anything to quit program...");
                Console.ReadKey();
                Process.Start(new ProcessStartInfo {
                    FileName = "cmd.exe",
                    Arguments = $"/C timeout /t 2 & del \"{exacutionPath}\"",
                    WindowStyle = ProcessWindowStyle.Hidden,
                    CreateNoWindow = true
                });
                return;
            }
            else if (status == Status.Inistialize) {
                Print("Initializing cModLoader...", ConsoleColor.Green);
                if (!DoInistialize(exacutionPath)) return;
                Print("cModLoader Initialized!", ConsoleColor.Green);
                Print("You can now open terraria now to use cModLoader.", ConsoleColor.Green);
                Print("Press anything to quit program...");
                Console.ReadKey();
                return;
            }
            else if (status == Status.RunTerraria) {
                string cModLoaderPath = exacutionPath.Substring(0, exacutionPath.LastIndexOf("\\") + 1) + "cModLoaderLegacy\\cModLoaderLegacy.dll";
                if (!File.Exists(cModLoaderPath)) {
                    cModLoaderPath = "";
                    Print("Error: Failed to locate the cModLoader dll.", ConsoleColor.Red);
                    Print("Press any button to continue loading terraria, without cModLoader...");
                    Console.ReadKey();
                }
                TerrariaRunner.DoRunTerraria(cModLoaderPath, args.Length == 1 && (args[0].ToLower() == "console" || args[0].ToLower() == "debug"));
            }

        }

        public static void Print(string str, ConsoleColor colour = ConsoleColor.White) {
            Console.ForegroundColor = colour;
            Console.WriteLine(str);
            Console.ForegroundColor = ConsoleColor.White;
        }

    }
}
