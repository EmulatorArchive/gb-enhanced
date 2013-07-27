if g++ -c config.cpp; then
	echo -e "Compiling Config...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Config...			\E[31m[ERROR]\E[37m"
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

if g++ -c gpu.cpp -lSDL; then
	echo -e "Compiling GB GPU...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling GB GPU...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -c source.cpp -lSDL; then
	echo -e "Compiling Main...			\E[32m[DONE]\E[37m"
else
	echo -e "Compiling Main...			\E[31m[ERROR]\E[37m"
	exit
fi

if g++ -o gbe config.o mmu.o z80.o gamepad.o gpu.o source.o -lSDL; then
	echo -e "Linking Project...			\E[32m[DONE]\E[37m"
else
	echo -e "Linking Project...			\E[31m[ERROR]\E[37m"
	exit
fi