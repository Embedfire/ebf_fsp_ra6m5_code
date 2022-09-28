SET PATH=C:\Keil_v5\ARM\ARMCLANG\Bin;E:\ProgramData\LLVM for Renesas RL78 10.0.0.202203\bin;E:\ProgramData\GCC for Renesas RX 8.3.0.202102-GNURX-ELF\bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files\Git\cmd;C:\Program Files\TortoiseGit\bin;C:\Program Files\dotnet\;C:\Users\admin\AppData\Local\Programs\Python\Python38\Scripts\;C:\Users\admin\AppData\Local\Programs\Python\Python38\;E:\ProgramData\LLVM for Renesas RL78 10.0.0.202203\bin;E:\ProgramData\GCC for Renesas RX 8.3.0.202102-GNURX-ELF\bin;C:\Users\admin\AppData\Local\Microsoft\WindowsApps;;C:\Users\admin\AppData\Local\Programs\Microsoft VS Code\bin;C:\Users\admin\.dotnet\tools
SET CPU_TYPE=R7FA6M5BH3CFC
SET CPU_VENDOR=Renesas
SET UV2_TARGET=Target 1
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hal_entry.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_led.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_key.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\startup.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\system.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_clocks.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_common.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_delay.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_group_irq.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_guard.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_io.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_irq.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_register_protection.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_rom_registers.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_sbrk.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\bsp_security.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\r_ioport.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\common_data.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hal_data.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\main.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\pin_data.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\vector_data.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via ".\Objects\EBF_RA6M5.lnp"
cmd /c "start "Renesas" /w cmd /c ""C:\Users\admin\AppData\Local\Arm\Packs\Renesas\RA_DFP\4.0.0\launcher\rasc_launcher.bat" "4.0.0" -nosplash --gensmartbundle --compiler ARMv6 "F:\fire_ra_code\ebf_fsp_ra6m5_code\EBF_Workspace_Keil\12_Key\configuration.xml" "F:\fire_ra_code\ebf_fsp_ra6m5_code\EBF_Workspace_Keil\12_Key\Objects\EBF_RA6M5.axf" 2> "%TEMP%\rasc_stderr.out"""
