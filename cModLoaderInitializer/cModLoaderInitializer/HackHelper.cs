using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace cModLoaderInitializerLegacy
{
    public static class HackHelper {

        public class DllHacker
        {
            [DllImport("kernel32.dll", SetLastError = true)] private static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);
            [DllImport("kernel32.dll", SetLastError = true)] private static extern IntPtr GetModuleHandle(string lpModuleName);
            [DllImport("kernel32.dll", SetLastError = true)] private static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, out IntPtr lpThreadId);
            [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)] private static extern IntPtr LoadLibrary(string lpFileName);
            [DllImport("kernel32", SetLastError = true)] private static extern bool FreeLibrary(IntPtr hModule);
            [DllImport("kernel32.dll", SetLastError = true)] private static extern bool WaitForSingleObject(IntPtr hHandle, uint dwMilliseconds);
            [DllImport("kernel32.dll")] private static extern bool CloseHandle(IntPtr hObject);

            public static void InjectDll(string DllPath, string initFucntionName)
            {
                SetThreadArgs(DllPath);

                IntPtr loadLibraryAddr = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
                if (loadLibraryAddr == IntPtr.Zero) {
                    FreeThreadArgs(); 
                    throw new Exception("Error loading dll");
                }

                IntPtr remoteThread = CreateRemoteThread(AttachedProcess.Handle, IntPtr.Zero, 0, loadLibraryAddr, threadArgs, 0, out _);
                if (remoteThread == IntPtr.Zero) {
                    FreeThreadArgs(); 
                    throw new Exception("Error creating remove thread");
                }

                WaitForSingleObject(remoteThread, 0xFFFFFFFF);
                CloseHandle(remoteThread);

                FreeThreadArgs();
            }

            private static Address threadArgs = 0;

            public static void SetThreadArgs(string str) => SetThreadArgs(Encoding.ASCII.GetBytes(str + "\0"));
            public static void SetThreadArgs(byte[] args)
            {
                FreeThreadArgs();
                threadArgs = Memory.AllocateMemory(4096);
                if (!Memory.WrightMemory(threadArgs, args))
                {
                    FreeThreadArgs();
                    throw new Exception("Error wrighting dll args to memory");
                }
            }
            public static void FreeThreadArgs()
            {
                if (threadArgs != 0) if (!Memory.FreeMemory(threadArgs.Value, 4096)) throw new Exception("Error freeing memory, while creating remove thread. " + Marshal.GetLastPInvokeErrorMessage());
                threadArgs = 0;
            }

            public static string RunDllFuctionAsThread(string DllPath, string funcName)
            {
                IntPtr selfLibAdd = LoadLibrary(DllPath);
                if (selfLibAdd == IntPtr.Zero)
                    return "Failed to Load Liberary, " + Marshal.GetLastPInvokeErrorMessage();

                IntPtr selfFuncAddress = GetProcAddress(selfLibAdd, funcName);
                if (selfFuncAddress == IntPtr.Zero) {
                    return "Function not found";
                }

                IntPtr libAdd = IntPtr.Zero;
                for (int i = 0; i < AttachedProcess.Modules.Count; i++)
                    if (AttachedProcess.Modules[i].ModuleName.ToLower() == DllPath.Substring(DllPath.LastIndexOf("\\") + 1).ToLower())
                        libAdd = AttachedProcess.Modules[i].BaseAddress;
                if (libAdd == IntPtr.Zero)
                    return ($"Dll '{DllPath}' not injected.");

                IntPtr funcAddress = libAdd + (selfFuncAddress - selfLibAdd);

                IntPtr funcRemoteThread = CreateRemoteThread(AttachedProcess.Handle, IntPtr.Zero, 0, funcAddress, threadArgs == 0 ? IntPtr.Zero : (IntPtr)threadArgs, 0, out _);
                if (funcRemoteThread == IntPtr.Zero)
                    return ("Error creating thread for function");

                WaitForSingleObject(funcRemoteThread, 0xFFFFFFFF);  // wait untill thread is finished
                CloseHandle(funcRemoteThread);
                return "";
            }

            public static void LoadDll(string DllPath)
            {
                IntPtr libAdd = LoadLibrary(DllPath);
                if (libAdd == IntPtr.Zero)
                {
                    throw new Exception("failed to Load Liberary " + Marshal.GetLastPInvokeErrorMessage());
                }
            }
            public static void UnLoadDll(string DllPath)
            {
                var Modules = Process.GetCurrentProcess().Modules;
                IntPtr libAdd = IntPtr.Zero;
                for (int i = 0; i < Modules.Count; i++)
                    if (Modules[i].ModuleName.ToLower() == DllPath.Substring(DllPath.LastIndexOf("\\") + 1).ToLower())
                        libAdd = Modules[i].BaseAddress;
                FreeLibrary(libAdd);
            }

            public static void MoveDll(string DllPath, string newPath)
            {
                if (!File.Exists(newPath))
                {
                    string dir = newPath.Substring(0, newPath.LastIndexOf("\\") + 1);
                    if (!Directory.Exists(dir))
                    {
                        Directory.CreateDirectory(dir);
                        try
                        {
                            File.Copy(DllPath, newPath, true);
                        }
                        catch (Exception e)
                        {
                            throw new Exception(e.Message + "\n" + e.StackTrace);
                        }
                    }
                }
                else
                {
                    try
                    {
                        UnLoadDll(DllPath);
                        UnLoadDll(newPath);
                        File.Copy(DllPath, newPath, true);
                    }
                    catch (Exception e)
                    {
                        throw new Exception(e.Message + "\n" + e.StackTrace);
                    }
                }
            }

        }

        private static Process? p = null;
        public static Process AttachedProcess
        {
            get
            {
                if (p == null) throw new Exception("Prossess was null.");
                return p;
            }
        }

        public static void AttachProsses(Process process) => p = process;
        public static Rectangle GetWindowRect() { Memory.WindowRect w = new Memory.WindowRect(); Memory.Win32Ref.GetWindowRect(AttachedProcess.MainWindowHandle, ref w); return Memory.WindowRect.ToRectangle(w); }

        public static class Memory
        {
            struct Privileges
            {
                public const uint PROCESS_CREATE_THREAD = 0x0002;
                public const uint PROCESS_QUERY_INFORMATION = 0x0400;
                public const uint PROCESS_VM_OPERATION = 0x0008;
                public const uint PROCESS_VM_WRITE = 0x0020;
                public const uint PROCESS_VM_READ = 0x0010;
            }
            struct ProcessAccessFlags
            {
                public const uint All = 0x001F0FFF;
                public const uint Terminate = 0x00000001;
                public const uint CreateThread = 0x00000002;
                public const uint VirtualMemoryOperation = 0x00000008;
                public const uint VirtualMemoryRead = 0x00000010;
                public const uint VirtualMemoryWrite = 0x00000020;
                public const uint DuplicateHandle = 0x00000040;
                public const uint CreateProcess = 0x000000080;
                public const uint SetQuota = 0x00000100;
                public const uint SetInformation = 0x00000200;
                public const uint QueryInformation = 0x00000400;
                public const uint QueryLimitedInformation = 0x00001000;
                public const uint Synchronize = 0x00100000;
            }
            /// <summary> Struct used for getting window rect only used for P/Invoke functions </summary>
            public struct WindowRect
            {
                public int Left { get; set; }
                public int Top { get; set; }
                public int Right { get; set; }
                public int Bottom { get; set; }
                public static Rectangle ToRectangle(WindowRect w) => new Rectangle(w.Left, w.Top, w.Right - w.Left, w.Bottom - w.Top);
            }
            /// <summary> The external functions that can be called to preform actions souch as wrighting or reading memory </summary>
            public static class Win32Ref
            {
                [DllImport("kernel32.dll", SetLastError = true)] public static extern bool VirtualProtectEx(IntPtr hProcess, UIntPtr lpAddress, IntPtr dwSize, IntPtr flNewProtect, out IntPtr lpflOldProtect);
                [DllImport("kernel32.dll", SetLastError = true)] public static extern bool ReadProcessMemory(IntPtr hProcess, UIntPtr lpBaseAddress, byte[] lpBuffer, UIntPtr nSize, out IntPtr lpNumberOfBytesRead);
                [DllImport("kernel32.dll", SetLastError = true)] public static extern int VirtualQueryEx(IntPtr hProcess, UIntPtr MemAddress, out MemInf lpBuffer, uint dwLength);
                [DllImport("kernel32.dll", SetLastError = true)] public static extern bool WriteProcessMemory(IntPtr hProcess, UIntPtr lpBaseAddress, byte[] lpBuffer, UIntPtr nSize, out IntPtr lpNumberOfBytesWritten);
                [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)] public static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint flAllocationType, uint flProtect);
                [DllImport("kernel32.dll", SetLastError = true)] public static extern bool VirtualFreeEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint dwFreeType);
                [DllImport("user32.dll", SetLastError = true)] public static extern bool GetWindowRect(IntPtr hwnd, ref WindowRect rectangle);
            }
            /// <summary> The type of memory being allocated </summary>
            public struct AlocationType
            {
                public const uint MEM_COMMIT = 0x00001000;
                public const uint MEM_RESERVE = 0x00002000;
                public const uint MEM_RESET = 0x00080000;
                public const uint MEM_RESET_UNDO = 0x1000000;
            }
            /// <summary> The protection type of memory </summary>
            public struct Protects
            {
                public const uint PAGE_EXECUTE = 0x10;
                public const uint PAGE_EXECUTE_READ = 0x20;
                public const uint PAGE_EXECUTE_READWRITE = 0x40;
                public const uint PAGE_EXECUTE_WRITECOPY = 0x80;
                public const uint PAGE_NOACCESS = 0x01;
                public const uint PAGE_READONLY = 0x02;
                public const uint PAGE_READWRITE = 0x04;
                public const uint PAGE_WRITECOPY = 0x08;
                public const uint PAGE_TARGETS_INVALID = 0x40000000;
                public const uint PAGE_TARGETS_NO_UPDATE = 0x40000000;
            }
            /// <summary> The memory info of the allocated memory section reaturned from the <see cref="Win32Ref.VirtualQueryEx(IntPtr, UIntPtr, out MemInf, uint)"/> function </summary>
            [StructLayout(LayoutKind.Sequential)]
            public struct MemInf
            {
                public UIntPtr BaseAddress;
                public UIntPtr AllocationBase;
                public ProtectEnum AllocationProtect;
                public UIntPtr RegionSize;
                public AllocationTypeEnum AlocationType;
                public ProtectEnum Protect;
                public TypeEnum Type;
                public enum TypeEnum : uint
                {
                    MEM_IMAGE = 0x1000000,
                    MEM_MAPPED = 0x40000,
                    MEM_PRIVATE = 0x20000
                }
                public enum ProtectEnum : uint
                {
                    PAGE_EXECUTE = 0x10,
                    PAGE_EXECUTE_READ = 0x20,
                    PAGE_EXECUTE_READWRITE = 0x40,
                    PAGE_EXECUTE_WRITECOPY = 0x80,
                    PAGE_NOACCESS = 0x01,
                    PAGE_READONLY = 0x02,
                    PAGE_READWRITE = 0x04,
                    PAGE_WRITECOPY = 0x08,
                    PAGE_TARGETS_INVALID = 0x40000000,
                    PAGE_TARGETS_NO_UPDATE = 0x40000000,
                }
                public enum AllocationTypeEnum : uint
                {
                    MEM_COMMIT = 0x1000,
                    MEM_RESERVE = 0x2000,
                    MEM_FREE = 0x10000,
                }
                public static uint size { get => (uint)Marshal.SizeOf(typeof(MemInf)); }
                public string ToString(bool hexValues = true) =>
                    !hexValues ?
                    $"Base: {BaseAddress}, Size: {RegionSize}, Protection: {Protect}, MemType: {Type}, AllocType: {AllocationProtect}" :
                    $"Base: {BaseAddress:x8}, Size: {RegionSize:x8}, Protection: {Protect}, MemType: {Type}, AllocType: {AllocationProtect}";
            }
            /// <summary> The comparison values used for the <see cref="ScanForBytes"/> function. </summary>
            public struct MemInfCompare
            {
                public MemInf.AllocationTypeEnum? AlocationType;
                public MemInf.ProtectEnum? Protect;
                public MemInf.ProtectEnum? AllocationProtect;
                public uint? RegionSize;
                public MemInf.TypeEnum? Type;
            }

            /// <summary>
            /// Scans the programs memory for an array of bytes from <paramref name="Min"/> to <paramref name="Max"/>.
            /// <para> Ceartin sections of the memory scaned can be compared to <paramref name="compare"/> to ignore memory sections like in unwrightable or erelivent memory </para>
            /// <para> Leave <paramref name="compare"/> as <see langword="null"/> if you do not know what memory you are scaning for. </para>
            /// <para> If <paramref name="compare"/> is not null and if <paramref name="ExactCompare"/> is true the <paramref name="compare"/> values are matched, if <paramref name="ExactCompare"/> is false, the <paramref name="compare"/> values then matched but inverted. </para>
            /// </summary>
            public static List<uint> ScanForBytes(byte[] arr, uint From = 0x1000000, uint To = 0x90000000, MemInfCompare? compare = null)
            {
                if (p == null) return new List<uint>();
                if (compare == null)
                    return ScanForBytes(arr, compare: new MemInfCompare()
                    {
                        AlocationType = MemInf.AllocationTypeEnum.MEM_COMMIT,
                        Protect = MemInf.ProtectEnum.PAGE_EXECUTE_READWRITE
                    });

                List<uint> longs = new List<uint>();
                uint CheckAddres = From;
                while (CheckAddres <= To)
                {
                    Win32Ref.VirtualQueryEx(p.Handle, (UIntPtr)CheckAddres, out MemInf m, (uint)Marshal.SizeOf(typeof(MemInf)));
                    if (compare.Value.AlocationType == null || m.AlocationType == compare.Value.AlocationType)
                        if (compare.Value.Protect == null || m.Protect == compare.Value.Protect)
                            if (compare.Value.AllocationProtect == null || m.AllocationProtect == compare.Value.AllocationProtect)
                                if (compare.Value.RegionSize == null || (uint)m.RegionSize == compare.Value.RegionSize)
                                    if (compare.Value.Type == null || m.Type == compare.Value.Type)
                                    {
                                        byte[] data = new byte[(int)m.RegionSize];
                                        Win32Ref.ReadProcessMemory(p.Handle, m.BaseAddress, data, m.RegionSize, out IntPtr intPtr);
                                        for (int i = 0; i < data.Length; i += 8)
                                            if (data[i] == arr[0])
                                                for (int j = 1; j < arr.Length; j++)
                                                    if (data[i + j] != arr[j]) break;
                                                    else if (j == arr.Length - 1)
                                                        longs.Add((uint)m.BaseAddress + (uint)i);
                                    }
                    CheckAddres = (uint)m.BaseAddress + (uint)m.RegionSize;
                }
                return longs;
            }

            public static bool WrightMemory(uint Address, byte[] bytes) => WrightMemory(new Address(Address), bytes, out uint ptr);
            public static bool WrightMemory(uint Address, byte[] bytes, out uint NumberOfBytesWrighten) => WrightMemory(new Address(Address), bytes, out NumberOfBytesWrighten);
            public static bool WrightMemory<T>(uint Address, T bytes) where T : struct => WrightMemory(new Address(Address), Converter.GetBytes(bytes), out uint ptr);

            public static bool WrightMemory(Address Address, byte[] bytes) => WrightMemory(Address, bytes, out uint ptr);
            public static bool WrightMemory(Address Address, byte[] bytes, out uint NumberOfBytesRead)
            {
                NumberOfBytesRead = 0;
                if (bytes == null) return false;
                bool b = Win32Ref.WriteProcessMemory(AttachedProcess.Handle, (UIntPtr)Address, bytes, (UIntPtr)bytes.Length, out IntPtr ptr);
                NumberOfBytesRead = (uint)ptr;
                return b;
            }
            public static bool WrightMemory<T>(Address Address, T value) where T : struct => WrightMemory(new Address(Address), Converter.GetBytes(value), out uint ptr);


            public static bool ReadMemory(uint Address, uint Length, out byte[] ReadBytes) => ReadMemory(new Address(Address), Length, out ReadBytes, out uint b);
            public static bool ReadMemory(uint Address, uint Length, out byte[] ReadBytes, out uint NumberOfBytesRead) => ReadMemory(new Address(Address), Length, out ReadBytes, out NumberOfBytesRead);
            public static bool ReadMemory<T>(uint Address, out T ReadValue, out uint NumberOfBytesRead) where T : struct => ReadMemory(new Address(Address), out ReadValue, out NumberOfBytesRead);

            public static bool ReadMemory(Address Address, uint Length, out byte[] ReadBytes) => ReadMemory(Address, Length, out ReadBytes, out uint b);
            public static bool ReadMemory(Address Address, uint Length, out byte[] ReadBytes, out uint NumberOfBytesRead)
            {
                ReadBytes = new byte[Length];
                bool b = Win32Ref.ReadProcessMemory(AttachedProcess.Handle, (UIntPtr)Address, ReadBytes, (UIntPtr)Length, out IntPtr Out);
                NumberOfBytesRead = (uint)Out;
                return b;
            }
            public static bool ReadMemory<T>(Address Address, out T ReadValue, out uint NumberOfBytesRead) where T : struct
            {
                ReadValue = new T();
                byte[] ReadBytes = new byte[Converter.GetSize<T>()];
                bool b = ReadMemory(new Address(Address), (uint)Converter.GetSize<T>(), out ReadBytes, out NumberOfBytesRead);
                if (!b) return false;
                ReadValue = Converter.GetValue<T>(ReadBytes);
                return true;
            }

            public static uint AllocateMemory(uint MemorySize, uint? AlocationTypeFlags = null, uint? Protection = null) => (uint)Win32Ref.VirtualAllocEx(AttachedProcess.Handle, IntPtr.Zero, MemorySize, AlocationTypeFlags ?? AlocationType.MEM_COMMIT | AlocationType.MEM_RESERVE, Protection ?? Protects.PAGE_READWRITE);
            public static Address AllocateMemoryA(uint MemorySize, uint? AlocationTypeFlags = null, uint? Protection = null) => (uint)Win32Ref.VirtualAllocEx(AttachedProcess.Handle, IntPtr.Zero, MemorySize, AlocationTypeFlags ?? AlocationType.MEM_COMMIT | AlocationType.MEM_RESERVE, Protection ?? Protects.PAGE_READWRITE);

            public static bool ChangeProtection(uint Address, uint Length, uint AlocationTypeFlags, out IntPtr OldAlocationType) => Win32Ref.VirtualProtectEx(AttachedProcess.Handle, Address, (IntPtr)Length, (IntPtr)AlocationTypeFlags, out OldAlocationType);
            public static bool ChangeProtection(Address Address, uint Length, uint AlocationTypeFlags, out IntPtr OldAlocationType) => Win32Ref.VirtualProtectEx(AttachedProcess.Handle, (UIntPtr)Address, (IntPtr)Length, (IntPtr)AlocationTypeFlags, out OldAlocationType);


            public static bool ClearMemory(uint Address, uint Length, out byte[] BytesCleared, byte CustomSet = 0x90, bool forceWrightMemory = false) => ClearMemory(new Address(Address), Length, out BytesCleared, CustomSet, forceWrightMemory);
            public static bool ClearMemory(Address Address, uint Length, out byte[] BytesCleared, byte CustomSet = 0x90, bool forceWrightMemory = false)
            {
                BytesCleared = new byte[Length];
                bool b = ReadMemory(Address, Length, out BytesCleared);
                if (!forceWrightMemory) if (!b) throw new ArgumentException("Unable To Read Memory");
                b = WrightMemory(Address, Enumerable.Repeat(CustomSet, (int)Length).ToArray());
                if (!b) throw new ArgumentException("Unable To Wright Memory");
                return true;
            }

            public static bool MoveMemory(uint Address, uint Length, int Movement, out uint NewAddress) => MoveMemory(new Address(Address), Length, Movement, out NewAddress);
            public static bool MoveMemory(Address Address, uint Length, int Movement, out uint NewAddress)
            {
                bool b = true;
                NewAddress = Address + (uint)Movement;
                ClearMemory(Address, Length, out byte[] bytes);
                b = WrightMemory(NewAddress, bytes);
                if (!b) throw new ArgumentException("Unable To Wright Memory");
                return b;
            }


            public static byte[] ReplaceMemory(uint Address, byte[] bytes) => ReplaceMemory(new Address(Address), bytes);
            public static byte[] ReplaceMemory(Address Address, byte[] bytes)
            {
                bool b = ReadMemory(Address, (uint)bytes.Length, out byte[] read);
                if (!b) return new byte[] { };
                WrightMemory(Address, bytes);
                return read;
            }


            private const uint MEM_RELEASE = 0x8000;
            private const uint MEM_DECOMMIT = 0x4000;
            public static bool FreeMemory(Address address, uint memorySize) => Win32Ref.VirtualFreeEx(AttachedProcess.Handle, (IntPtr)((uint)address.Value), memorySize, MEM_DECOMMIT);

        }
        public static class Converter
        {
            /// <summary>
            /// Converts a hex string souch as <c>"E4 45 C5 60"</c> to a byte array
            /// <para>All spaces will be removed, so it will convert string souch as <c>"1D 54 427C00 0 0"</c></para>
            /// <para>WARNING: <paramref name="str"/> needs to be of an even length (not including spaces)</para>
            /// </summary>
            /// <returns> the data in <paramref name="str"/> in byte array </returns>
            public static byte[] ToByteArr(string str) => string.Join("", str.Replace(" ", "").Select((x, i) => (i % 2 == 0) ? $" {x}" : $"{x}")).Trim().Split(" ").Select(x => Convert.ToByte(x, 16)).ToArray();

            /// <summary>
            /// Converts an array of bytes (<paramref name="bytes"/>) to a value of type <typeparamref name="T"/>.
            /// <para>Valid types: <see cref="bool"/>, <see cref="char"/>, <see cref="Half"/>, <see cref="short"/>, <see cref="ushort"/>, <see cref="int"/>, <see cref="uint"/>, <see cref="long"/>, <see cref="ulong"/>, <see cref="double"/>, <see cref="float"/>, <see cref="Address"/>, <see cref="IntPtr"/>, <see cref="UIntPtr"/> </para>
            /// <para>This just uses <see cref="BitConverter"/>, but its here. </para>
            /// </summary>
            /// <returns> The converted value of <paramref name="bytes"/>, or a new instance of <typeparamref name="T"/> if <typeparamref name="T"/> is not part of the list of valid types. </returns>
            public static T GetValue<T>(byte[] val) where T : struct
            {

                if (typeof(T) == typeof(bool)) return (T)(object)BitConverter.ToBoolean(val);
                if (typeof(T) == typeof(byte)) return (T)(object)val[0];
                if (typeof(T) == typeof(sbyte)) return (T)(object)val[0];
                if (typeof(T) == typeof(char)) return (T)(object)BitConverter.ToChar(val);
                if (typeof(T) == typeof(Half)) return (T)(object)BitConverter.ToHalf(val);
                if (typeof(T) == typeof(short)) return (T)(object)BitConverter.ToInt16(val);
                if (typeof(T) == typeof(ushort)) return (T)(object)BitConverter.ToUInt16(val);
                if (typeof(T) == typeof(int)) return (T)(object)BitConverter.ToInt32(val);
                if (typeof(T) == typeof(uint)) return (T)(object)BitConverter.ToUInt32(val);
                if (typeof(T) == typeof(long)) return (T)(object)BitConverter.ToInt64(val);
                if (typeof(T) == typeof(ulong)) return (T)(object)BitConverter.ToUInt64(val);
                if (typeof(T) == typeof(double)) return (T)(object)BitConverter.ToDouble(val);
                if (typeof(T) == typeof(float)) return (T)(object)BitConverter.ToSingle(val);
                if (typeof(T) == typeof(Address)) return (T)(object)new Address(BitConverter.ToUInt64(val));
                if (typeof(T) == typeof(IntPtr)) return (T)(object)BitConverter.ToInt32(val);
                if (typeof(T) == typeof(UIntPtr)) return (T)(object)BitConverter.ToUInt32(val);
                return new T();
            }
            /// <summary>
            /// Converts an array of bytes (<paramref name="bytes"/>) to a value of type <paramref name="ToType"/>
            /// <para>Valid types: <see cref="bool"/>, <see cref="char"/>, <see cref="Half"/>, <see cref="short"/>, <see cref="ushort"/>, <see cref="int"/>, <see cref="uint"/>, <see cref="long"/>, <see cref="ulong"/>, <see cref="double"/>, <see cref="float"/>, <see cref="Address"/>, <see cref="IntPtr"/>, <see cref="UIntPtr"/> </para>
            /// <para>This usalized the <see cref="Type"/> class instead of using <typeparamref name="T"/></para>
            /// </summary>
            /// <returns> The converted value of <paramref name="bytes"/>, or a new instance of <typeparamref name="T"/> if <see cref="Type"/> is not part of the list of valid types. </returns>
            public static T GetValue<T>(Type ToType, byte[] val)
            {
                if (ToType == typeof(bool)) return (T)(object)BitConverter.ToBoolean(val);
                if (ToType == typeof(byte)) return (T)(object)val[0];
                if (ToType == typeof(sbyte)) return (T)(object)val[0];
                if (ToType == typeof(char)) return (T)(object)BitConverter.ToChar(val);
                if (ToType == typeof(Half)) return (T)(object)BitConverter.ToHalf(val);
                if (ToType == typeof(short)) return (T)(object)BitConverter.ToInt16(val);
                if (ToType == typeof(ushort)) return (T)(object)BitConverter.ToUInt16(val);
                if (ToType == typeof(int)) return (T)(object)BitConverter.ToInt32(val);
                if (ToType == typeof(uint)) return (T)(object)BitConverter.ToUInt32(val);
                if (ToType == typeof(long)) return (T)(object)BitConverter.ToInt64(val);
                if (ToType == typeof(ulong)) return (T)(object)BitConverter.ToUInt64(val);
                if (ToType == typeof(double)) return (T)(object)BitConverter.ToDouble(val);
                if (ToType == typeof(float)) return (T)(object)BitConverter.ToSingle(val);
                if (ToType == typeof(Address)) return (T)(object)BitConverter.ToUInt64(val);
                if (ToType == typeof(IntPtr)) return (T)(object)BitConverter.ToInt32(val);
                if (ToType == typeof(UIntPtr)) return (T)(object)BitConverter.ToUInt32(val);
                return (T)new object();
            }
            /// <summary>
            /// Converts a value of type <typeparamref name="T"/> to an array of bytes.
            /// <para>Valid types: <see cref="bool"/>, <see cref="char"/>, <see cref="Half"/>, <see cref="short"/>, <see cref="ushort"/>, <see cref="int"/>, <see cref="uint"/>, <see cref="long"/>, <see cref="ulong"/>, <see cref="double"/>, <see cref="float"/>, <see cref="Address"/>, <see cref="IntPtr"/>, <see cref="UIntPtr"/> </para>
            /// <para>This just uses <see cref="BitConverter"/>, but its here. </para>
            /// </summary>
            /// <returns> The converted value of <paramref name="bytes"/>, or an empty array if <typeparamref name="T"/> is not part of the list of valid types. </returns>
            public static byte[] GetBytes<T>(T val) where T : struct
            {
                if (typeof(T) == typeof(bool)) return BitConverter.GetBytes((bool)(object)val);
                if (typeof(T) == typeof(char)) return BitConverter.GetBytes((char)(object)val);
                if (typeof(T) == typeof(Half)) return BitConverter.GetBytes((Half)(object)val);
                if (typeof(T) == typeof(short)) return BitConverter.GetBytes((short)(object)val);
                if (typeof(T) == typeof(ushort)) return BitConverter.GetBytes((ushort)(object)val);
                if (typeof(T) == typeof(int)) return BitConverter.GetBytes((int)(object)val);
                if (typeof(T) == typeof(uint)) return BitConverter.GetBytes((uint)(object)val);
                if (typeof(T) == typeof(long)) return BitConverter.GetBytes((long)(object)val);
                if (typeof(T) == typeof(ulong)) return BitConverter.GetBytes((ulong)(object)val);
                if (typeof(T) == typeof(double)) return BitConverter.GetBytes((double)(object)val);
                if (typeof(T) == typeof(float)) return BitConverter.GetBytes((float)(object)val);
                if (typeof(T) == typeof(Address)) return BitConverter.GetBytes((Address)(object)val);
                if (typeof(T) == typeof(IntPtr)) return BitConverter.GetBytes((int)(object)val);
                if (typeof(T) == typeof(UIntPtr)) return BitConverter.GetBytes((uint)(object)val);
                return new byte[] { };
            }
            /// <summary> Returns the amount of bytes the type <typeparamref name="T"/>. This is relitive to what i think, wich is what google sais, wich is for reason diffrent then <see cref="Marshal.SizeOf{T}()"/>
            /// <para>Valid types: <see cref="bool"/>, <see cref="char"/>, <see cref="Half"/>, <see cref="short"/>, <see cref="ushort"/>, <see cref="int"/>, <see cref="uint"/>, <see cref="long"/>, <see cref="ulong"/>, <see cref="double"/>, <see cref="float"/>, <see cref="Address"/>, <see cref="IntPtr"/>, <see cref="UIntPtr"/> </para>
            /// </summary>
            /// <returns> The byte size of <typeparamref name="T"/> </returns>
            public static int GetSize<T>() where T : struct
            {
                if (typeof(T) == typeof(bool)) return 1;
                if (typeof(T) == typeof(byte)) return 1;
                if (typeof(T) == typeof(sbyte)) return 1;
                if (typeof(T) == typeof(char)) throw new Exception("unimplemented char size, 1 or 2");
                if (typeof(T) == typeof(Half)) return 2;
                if (typeof(T) == typeof(short)) return 2;
                if (typeof(T) == typeof(ushort)) return 2;
                if (typeof(T) == typeof(int)) return 4;
                if (typeof(T) == typeof(uint)) return 4;
                if (typeof(T) == typeof(long)) return 8;
                if (typeof(T) == typeof(ulong)) return 8;
                if (typeof(T) == typeof(double)) return 8;
                if (typeof(T) == typeof(float)) return 4;
                if (typeof(T) == typeof(Address)) return 8;
                if (typeof(T) == typeof(IntPtr)) return 4;
                if (typeof(T) == typeof(UIntPtr)) return 4;
                return 0;
            }
        }
        /// <summary> Used like any normal numaric struct, stores the value as ulong. Used mainly for P/Invoke functions as the address </summary>
        public struct Address
        {
            public const ulong MaxValue = 0xffffffffffffffffL;
            public const ulong MinValue = 0;

            public ulong Value = 0;

            public Address(sbyte v) => Value = (ulong)v;
            public Address(byte v) => Value = v;
            public Address(short v) => Value = (ulong)v;
            public Address(ushort v) => Value = v;
            public Address(int v) => Value = (ulong)v;
            public Address(uint v) => Value = v;
            public Address(long v) => Value = (ulong)v;
            public Address(ulong v) => Value = v;
            public Address(Address v) => Value = v.Value;

            public string ToString() => Value.ToString();
            public string ToHexString() => Value.ToString("x8");
            public string ToUpHexString() => Value.ToString("x8").ToUpper();

            public static implicit operator Address(sbyte i) => new Address(i < 0 ? 0 : i);
            public static implicit operator Address(byte i) => new Address(i < 0 ? 0 : i);
            public static implicit operator Address(short i) => new Address(i < 0 ? 0 : i);
            public static implicit operator Address(ushort i) => new Address(i < 0 ? 0 : i);
            public static implicit operator Address(int i) => new Address(i < 0 ? 0 : i);
            public static implicit operator Address(uint i) => new Address(i < 0 ? 0 : i);
            public static implicit operator Address(long i) => new Address(i < 0 ? 0 : i);
            public static implicit operator Address(ulong i) => new Address(i < 0 ? 0 : i);

            private static Address add(object c1, Address c2) => add(c2, c1);
            private static Address sub(object c1, Address c2) => sub(c2, c1);
            private static Address add(Address c1, object c2)
            {
                string num = "";
                if (c2 is sbyte sb) num = sb.ToString();
                else if (c2 is byte b) num = b.ToString();
                else if (c2 is short s) num = s.ToString();
                else if (c2 is ushort us) num = us.ToString();
                else if (c2 is int i) num = i.ToString();
                else if (c2 is uint ui) num = ui.ToString();
                else if (c2 is long l) num = l.ToString();
                else if (c2 is ulong ul) num = ul.ToString();
                else if (c2 is IntPtr ip) num = ip.ToString();
                else if (c2 is UIntPtr uip) num = uip.ToString();
                else if (c2 is Address a) num = a.ToString();
                else throw new ArgumentException("Unsupported type", nameof(c2));
                if (num.Contains("-"))
                    try { return new Address(c1.Value - ulong.Parse(long.Parse(num).ToString().Substring(1))); }
                    catch (Exception) { throw new ArgumentException("Incompatible Value", nameof(c2)); }
                else
                    try { return new Address(c1.Value + ulong.Parse(num)); }
                    catch (Exception) { throw new ArgumentException("Incompatible Value", nameof(c2)); }
            }
            private static Address sub(Address c1, object c2)
            {
                string num = "";
                if (c2 is sbyte sb) num = sb.ToString();
                else if (c2 is byte b) num = b.ToString();
                else if (c2 is short s) num = s.ToString();
                else if (c2 is ushort us) num = us.ToString();
                else if (c2 is int i) num = i.ToString();
                else if (c2 is uint ui) num = ui.ToString();
                else if (c2 is long l) num = l.ToString();
                else if (c2 is ulong ul) num = ul.ToString();
                else if (c2 is IntPtr ip) num = ip.ToString();
                else if (c2 is UIntPtr uip) num = uip.ToString();
                else if (c2 is Address a) num = a.ToString();
                else throw new ArgumentException("Unsupported type", nameof(c2));
                if (num.Contains("-"))
                    try { return new Address(c1.Value + ulong.Parse(long.Parse(num).ToString().Substring(1))); }
                    catch (Exception) { throw new ArgumentException("Incompatible Value", nameof(c2)); }
                else
                    try { return new Address(c1.Value - ulong.Parse(num)); }
                    catch (Exception) { throw new ArgumentException("Incompatible Value", nameof(c2)); }
            }

            public static bool operator ==(object c1, Address c2) => c2 == c1;
            public static bool operator !=(object c1, Address c2) => c2 != c1;
            public static bool operator ==(Address c1, object c2)
            {
                string num = "";
                if (c2 is sbyte sb) num = sb.ToString();
                else if (c2 is byte b) num = b.ToString();
                else if (c2 is short s) num = s.ToString();
                else if (c2 is ushort us) num = us.ToString();
                else if (c2 is int i) num = i.ToString();
                else if (c2 is uint ui) num = ui.ToString();
                else if (c2 is long l) num = l.ToString();
                else if (c2 is ulong ul) num = ul.ToString();
                else if (c2 is IntPtr ip) num = ip.ToString();
                else if (c2 is UIntPtr uip) num = uip.ToString();
                else if (c2 is Address a) num = a.ToString();
                else throw new ArgumentException("Unsupported type", nameof(c2));
                return num == c1.ToString();
            }
            public static bool operator !=(Address c1, object c2) => !(c2 == c1);

            public static implicit operator int(Address address)
            {
                if (address.Value > int.MaxValue) throw new ArgumentException("Value To Large", nameof(address));
                return (int)address.Value;
            }
            public static implicit operator uint(Address address)
            {
                if (address.Value > uint.MaxValue) throw new ArgumentException("Value To Large", nameof(address));
                return (uint)address.Value;
            }
            public static implicit operator long(Address address)
            {
                if (address.Value > long.MaxValue) throw new ArgumentException("Value To Large", nameof(address));
                return (long)address.Value;
            }
            public static implicit operator ulong(Address address) => address.Value;
            public static implicit operator IntPtr(Address address)
            {
                if (address.Value > (ulong)IntPtr.MaxValue) throw new ArgumentException("Value To Large", nameof(address));
                return (IntPtr)address.Value;
            }
            public static implicit operator UIntPtr(Address address)
            {
                if (address.Value > (ulong)UIntPtr.MaxValue) throw new ArgumentException("Value To Large", nameof(address));
                return (UIntPtr)address.Value;
            }




            public static Address operator +(Address c1, Address c2) => add(c2, (object)c1);
            public static Address operator +(sbyte c1, Address c2) => add(c2, (object)c1);
            public static Address operator +(byte c1, Address c2) => add(c2, (object)c1);
            public static Address operator +(short c1, Address c2) => add(c2, (object)c1);
            public static Address operator +(ushort c1, Address c2) => add(c2, (object)c1);
            public static Address operator +(int c1, Address c2) => add(c2, (object)c1);
            public static Address operator +(uint c1, Address c2) => add(c2, (object)c1);
            public static Address operator +(long c1, Address c2) => add(c2, (object)c1);
            public static Address operator +(ulong c1, Address c2) => add(c2, (object)c1);
            public static Address operator +(IntPtr c1, Address c2) => add(c2, (object)c1);
            public static Address operator +(UIntPtr c1, Address c2) => add(c2, (object)c1);

            public static Address operator +(Address c2, sbyte c1) => add(c2, (object)c1);
            public static Address operator +(Address c2, byte c1) => add(c2, (object)c1);
            public static Address operator +(Address c2, short c1) => add(c2, (object)c1);
            public static Address operator +(Address c2, ushort c1) => add(c2, (object)c1);
            public static Address operator +(Address c2, int c1) => add(c2, (object)c1);
            public static Address operator +(Address c2, uint c1) => add(c2, (object)c1);
            public static Address operator +(Address c2, long c1) => add(c2, (object)c1);
            public static Address operator +(Address c2, ulong c1) => add(c2, (object)c1);
            public static Address operator +(Address c2, IntPtr c1) => add(c2, (object)c1);
            public static Address operator +(Address c2, UIntPtr c1) => add(c2, (object)c1);

            public static Address operator -(Address c2, sbyte c1) => sub(c2, (object)c1);
            public static Address operator -(Address c2, byte c1) => sub(c2, (object)c1);
            public static Address operator -(Address c2, short c1) => sub(c2, (object)c1);
            public static Address operator -(Address c2, ushort c1) => sub(c2, (object)c1);
            public static Address operator -(Address c2, int c1) => sub(c2, (object)c1);
            public static Address operator -(Address c2, uint c1) => sub(c2, (object)c1);
            public static Address operator -(Address c2, long c1) => sub(c2, (object)c1);
            public static Address operator -(Address c2, ulong c1) => sub(c2, (object)c1);
            public static Address operator -(Address c2, IntPtr c1) => sub(c2, (object)c1);
            public static Address operator -(Address c2, UIntPtr c1) => sub(c2, (object)c1);
            public static Address operator -(Address c2, Address c1) => sub(c2, (object)c1);


            public static bool operator !=(Address c1, Address c2) => c2 != (object)c1;
            public static bool operator ==(Address c1, Address c2) => c2 == (object)c1;

            public static bool operator ==(sbyte c1, Address c2) => c2 == (object)c1;
            public static bool operator ==(byte c1, Address c2) => c2 == (object)c1;
            public static bool operator ==(short c1, Address c2) => c2 == (object)c1;
            public static bool operator ==(ushort c1, Address c2) => c2 == (object)c1;
            public static bool operator ==(int c1, Address c2) => c2 == (object)c1;
            public static bool operator ==(uint c1, Address c2) => c2 == (object)c1;
            public static bool operator ==(long c1, Address c2) => c2 == (object)c1;
            public static bool operator ==(ulong c1, Address c2) => c2 == (object)c1;
            public static bool operator ==(IntPtr c1, Address c2) => c2 == (object)c1;
            public static bool operator ==(UIntPtr c1, Address c2) => c2 == (object)c1;

            public static bool operator !=(sbyte c1, Address c2) => c2 != (object)c1;
            public static bool operator !=(byte c1, Address c2) => c2 != (object)c1;
            public static bool operator !=(short c1, Address c2) => c2 != (object)c1;
            public static bool operator !=(ushort c1, Address c2) => c2 != (object)c1;
            public static bool operator !=(int c1, Address c2) => c2 != (object)c1;
            public static bool operator !=(uint c1, Address c2) => c2 != (object)c1;
            public static bool operator !=(long c1, Address c2) => c2 != (object)c1;
            public static bool operator !=(ulong c1, Address c2) => c2 != (object)c1;
            public static bool operator !=(IntPtr c1, Address c2) => c2 != (object)c1;
            public static bool operator !=(UIntPtr c1, Address c2) => c2 != (object)c1;

            public static bool operator ==(Address c2, sbyte c1) => c2 == (object)c1;
            public static bool operator ==(Address c2, byte c1) => c2 == (object)c1;
            public static bool operator ==(Address c2, short c1) => c2 == (object)c1;
            public static bool operator ==(Address c2, ushort c1) => c2 == (object)c1;
            public static bool operator ==(Address c2, int c1) => c2 == (object)c1;
            public static bool operator ==(Address c2, uint c1) => c2 == (object)c1;
            public static bool operator ==(Address c2, long c1) => c2 == (object)c1;
            public static bool operator ==(Address c2, ulong c1) => c2 == (object)c1;
            public static bool operator ==(Address c2, IntPtr c1) => c2 == (object)c1;
            public static bool operator ==(Address c2, UIntPtr c1) => c2 == (object)c1;

            public static bool operator !=(Address c2, sbyte c1) => c2 != (object)c1;
            public static bool operator !=(Address c2, byte c1) => c2 != (object)c1;
            public static bool operator !=(Address c2, short c1) => c2 != (object)c1;
            public static bool operator !=(Address c2, ushort c1) => c2 != (object)c1;
            public static bool operator !=(Address c2, int c1) => c2 != (object)c1;
            public static bool operator !=(Address c2, uint c1) => c2 != (object)c1;
            public static bool operator !=(Address c2, long c1) => c2 != (object)c1;
            public static bool operator !=(Address c2, ulong c1) => c2 != (object)c1;
            public static bool operator !=(Address c2, IntPtr c1) => c2 != (object)c1;
            public static bool operator !=(Address c2, UIntPtr c1) => c2 != (object)c1;

            public override bool Equals(object obj)
            {
                throw new NotImplementedException();
            }

            public override int GetHashCode()
            {
                throw new NotImplementedException();
            }
        }

    }
}
