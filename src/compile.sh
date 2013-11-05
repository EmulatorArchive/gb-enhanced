if g++ -c config.cpp; then
	echo -e "Compiling Config...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Config...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c mbc1.cpp; then
	echo -e "Compiling MBC1...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling MBC1...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c mbc2.cpp; then
	echo -e "Compiling MBC2...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling MBC2...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c mbc3.cpp; then
	echo -e "Compiling MBC3...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling MBC3...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c mbc5.cpp; then
	echo -e "Compiling MBC5...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling MBC5...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c mmu.cpp; then
	echo -e "Compiling MMU...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling MMU...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c z80.cpp; then
	echo -e "Compiling Z80 CPU...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Z80 CPU...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c gamepad.cpp -lSDL; then
	echo -e "Compiling Game Pad...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Game Pad...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c filter.cpp -lSDL; then
	echo -e "Compiling Filters...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Filters...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c gpu.cpp -lSDL; then
	echo -e "Compiling GB GPU...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling GB GPU...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c hotkeys.cpp -lSDL; then
	echo -e "Compiling Hotkeys...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Hotkeys...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c source.cpp -lSDL; then
	echo -e "Compiling Main...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Main...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -o gbe config.o mbc1.o mbc2.o mbc3.o mbc5.o mmu.o z80.o gamepad.o filter.o gpu.o hotkeys.o source.o -lSDL; then
	echo -e "Linking Project...			\E[32m[DONE]\E[37m"
else
	echo -e "Linking Project...			\E[31m[ERROR]\E[37m"
	exit
fi