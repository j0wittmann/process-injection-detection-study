# Process Injection

This directory contains the implementations of the process injection techniques evaluated in the bachelor’s thesis.

The following techniques are implemented:

- DLL Injection (T1055.001)
- PE Injection (T1055.002)
- Thread Execution Hijacking (T1055.003)
- APC Injection (T1055.004 – standard and Early Bird)
- Local Shellcode Injection (standard WinAPI execution and direct/indirect syscall variants)

The injected shellcode was embedded directly or fetched at runtime via staged logic.

For reference, see the MITRE ATT&CK (T1055):
https://attack.mitre.org/techniques/T1055/
