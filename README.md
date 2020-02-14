# Apple 1 Emulator

Apple 1 emulator based on [OneLoneCoder NES emulation components](https://github.com/OneLoneCoder/olcNES) and [olcPixelGameEngine](https://github.com/OneLoneCoder/olcPixelGameEngine), Copyright 2018, 2019 OneLoneCoder.com. With this project I want to adapt and enhance what I learned with my own [C# based Apple 1 emulator](https://github.com/KaiWalter/CS8BitEmulators/blob/master/MyEMU/Systems/Apple1.cs).

Borrowed test ROMs from https://github.com/Klaus2m5/6502_65C02_functional_tests.

> Feb 2020 : This is just an exercise for me to brush up my C++ skills which had been dormant for over 2 decades.

## Apple 1 with Hex Monitor

![](/img/Apple1_HexMon.png)

## compile in Linux

```
g++ -o olcApple1 ./*.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17
```