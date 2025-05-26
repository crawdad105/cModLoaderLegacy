using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cModLoaderInitializerLegacy
{
    public static class TerrariaRunner
    {
        public static void DoRunTerraria(string cModLoaderPath, bool consoleMode) {
            Program.Print("Inistializing Terraria...", ConsoleColor.Yellow);
            ProcessStartInfo psi = new ProcessStartInfo
            {
                FileName = "RealTerraria.exe",
                Arguments = "",
                UseShellExecute = !consoleMode,
                RedirectStandardOutput = consoleMode,
                RedirectStandardError = consoleMode,
                CreateNoWindow = false
            };

            Process terraria = new Process { StartInfo = psi };
            if (consoleMode) {
                terraria.OutputDataReceived += OnOuput;
                terraria.ErrorDataReceived += OnError;
            }
            Program.Print("Starting Terraria...", ConsoleColor.Yellow);
            terraria.Start();
            Program.Print("Waiting for Terraria to start...", ConsoleColor.Yellow);
            if (consoleMode) {
                terraria.BeginOutputReadLine();
                terraria.BeginErrorReadLine();
            }
            while (terraria.MainWindowHandle == 0) Thread.Sleep(1000);
            Thread.Sleep(500);

            HackHelper.AttachProsses(terraria);
            HackHelper.DllHacker.InjectDll(cModLoaderPath, "");
            string err = HackHelper.DllHacker.RunDllFuctionAsThread(cModLoaderPath, "HookMainMenuInterface");
            if (err != "") {
                Program.Print("An error occured while running remote thread.", ConsoleColor.Red);
                Console.WriteLine(err);
                Program.Print("Press anything to quit program...");
                Console.ReadKey();
            }
            
            if (consoleMode) {
                terraria.WaitForExit();
                Program.Print("Terraria closed.", ConsoleColor.Yellow);
                Program.Print("Press anything to quit program...");
                Console.ReadKey();
            }
        }

        private static void OnOuput(object sender, DataReceivedEventArgs e) {
            Program.Print(e.Data ?? "");
        }
        private static void OnError(object sender, DataReceivedEventArgs e){
            Program.Print("Error: " + (e.Data ?? ""), ConsoleColor.Red);
        }
    }
}
