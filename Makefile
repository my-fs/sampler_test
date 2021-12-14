build:
	g++ -std=c++17 \
	-arch x86_64 \
	-ggdb \
	-I/usr/local/Homebrew/include \
	-L/usr/local/Homebrew/lib \
	-Iinclude \
	src/main.cpp \
	-lSDL2 \
	-lSDL2_image \
	-o play