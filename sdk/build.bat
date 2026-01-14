set path=D:\Keil_v5\ARM\ARMCC\bin
fromelf.exe --bin -o ./Output/bsr1901_demo.bin ./Objects/bsr1901_demo.axf
fromelf.exe -rc -o ./Output/bsr1901_demo.asm ./Objects/bsr1901_demo.axf
fromelf.exe -ad -o ./Output/bsr1901_demo.data ./Objects/bsr1901_demo.axf
fromelf.exe -cvf ./Objects/bsr1901_demo.axf --vhx --32x1 -o ./Output/bsr1901.hex
fromelf.exe -cvf ./Objects/bsr1901_demo.axf -o ./Output/bsr1901_asm.txt


