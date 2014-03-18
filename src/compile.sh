if g++ -c -O3 -funroll-loops config.cpp; then
	echo -e "Compiling Config...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Config...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops hash.cpp; then
	echo -e "Compiling Hash...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Hash...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops mbc1.cpp; then
	echo -e "Compiling MBC1...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling MBC1...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops mbc2.cpp; then
	echo -e "Compiling MBC2...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling MBC2...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops mbc3.cpp; then
	echo -e "Compiling MBC3...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling MBC3...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops mbc5.cpp; then
	echo -e "Compiling MBC5...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling MBC5...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops mmu.cpp; then
	echo -e "Compiling MMU...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling MMU...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops z80.cpp; then
	echo -e "Compiling Z80 CPU...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Z80 CPU...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops gamepad.cpp -lSDL; then
	echo -e "Compiling Game Pad...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Game Pad...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops filter.cpp -lSDL; then
	echo -e "Compiling Filters...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Filters...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops gpu.cpp -lSDL; then
	echo -e "Compiling GB GPU...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling GB GPU...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops apu.cpp -lSDL; then
	echo -e "Compiling GB APU...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling GB APU...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops hotkeys.cpp -lSDL; then
	echo -e "Compiling Hotkeys...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Hotkeys...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops opengl.cpp -lSDL -lGL; then
	echo -e "Compiling OpenGL...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling OpenGL...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c -O3 -funroll-loops custom_gfx.cpp -lSDL; then
	echo -e "Compiling Custom GFX...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Custom GFX...			\E[31m[ERROR]\E[37m"
	exit
fi


if g++ -c -O3 -funroll-loops source.cpp -lSDL; then
	echo -e "Compiling Main...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Main...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -o gbe config.o hash.o mbc1.o mbc2.o mbc3.o mbc5.o mmu.o z80.o gamepad.o filter.o gpu.o apu.o hotkeys.o opengl.o custom_gfx.o source.o -lSDL -lGL; then
	echo -e "Linking Project...			\E[32m[DONE]\E[37m"
else
	echo -e "Linking Project...			\E[31m[ERROR]\E[37m"
	exit
fi