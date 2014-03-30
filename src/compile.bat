g++ -c -O3 -funroll-loops config.cpp
g++ -c -O3 -funroll-loops hash.cpp
g++ -c -O3 -funroll-loops mbc1.cpp
g++ -c -O3 -funroll-loops mbc2.cpp
g++ -c -O3 -funroll-loops mbc3.cpp
g++ -c -O3 -funroll-loops mbc5.cpp
g++ -c -O3 -funroll-loops mmu.cpp
g++ -c -O3 -funroll-loops z80.cpp
g++ -c -O3 -funroll-loops gamepad.cpp -lmingw32 -lSDLmain -lSDL
g++ -c -O3 -funroll-loops filter.cpp -lmingw32 -lSDLmain -lSDL
g++ -c -O3 -funroll-loops gpu.cpp -lmingw32 -lSDLmain -lSDL
g++ -c -O3 -funroll-loops apu.cpp -lmingw32 -lSDLmain -lSDL
g++ -c -O3 -funroll-loops hotkeys.cpp -lmingw32 -lSDLmain -lSDL
g++ -c -O3 -funroll-loops opengl.cpp -lmingw32 -lSDLmain -lSDL -lopengl32
g++ -c -O3 -funroll-loops custom_gfx.cpp -lmingw32 -lSDLmain -lSDL
g++ -c -O3 -funroll-loops source.cpp -lmingw32 -lSDLmain -lSDL
g++ -o gbe.exe config.o hash.o mbc1.o mbc2.o mbc3.o mbc5.o mmu.o z80.o gamepad.o filter.o gpu.o apu.o hotkeys.o opengl.o custom_gfx.o source.o -lmingw32 -lSDLmain -lSDL -lopengl32