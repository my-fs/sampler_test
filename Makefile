build:
	g++ -std=c++17 \
	-arch x86_64 \
	-ggdb \
	-I/usr/local/Homebrew/include \
	-L/usr/local/Homebrew/lib \
	-I/usr/local/Homebrew/include/rtmidi \
	-Iinclude \
	src/playground.cpp \
	src/midifile/Binasc.cpp \
	src/midifile/MidiEvent.cpp \
	src/midifile/MidiEventList.cpp \
	src/midifile/MidiFile.cpp \
	src/midifile/MidiMessage.cpp \
	src/midifile/Options.cpp \
	-lSDL2 \
	-lSDL2_image \
	-lrtmidi \
	-o play
