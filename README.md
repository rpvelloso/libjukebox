# libjukebox
Plataform independent (windows/linux) C++ audio playback library (uses DirectSound under Windows and Alsa under Linux).

# Supported file formats:
- RAW Wave;
- MP3;
- Ogg Vorbis;
- MIDI (using libfluidsynth);
- Extensible architecture allows implementation of other formats, transparently.

# Effects
- Fade in/out;
- Fade on stop;
- Extensible architecture allows implementation of custom effects.

# Example
```cpp
#include <string>
#include <iostream>
#include "libjukebox.h"

void printFileData(const jukebox::SoundFile &file) {
	std::cout << file.getFilename() << " attributes: " << std::endl;
	std::cout << file.getBitsPerSample() << " bits" << std::endl;
	std::cout << file.getNumChannels() << " channel(s)" << std::endl;
	std::cout << file.getSampleRate() << " Hz" << std::endl;
	std::cout << file.getDataSize() << " bytes" << std::endl;
	std::cout << file.getDuration() << " sec(s)" << std::endl << std::endl;
}

int main(int argc, char **argv) {
	if( argc < 2 ) {
		std::cout << "usage: " << argv[0] << " filename.[wav|ogg]" << std::endl;
		return 1;
	}

	std::string filename(argv[1]);

	auto soundFile = filename.back() == 'g'? // ogg?
			jukebox::factory::loadVorbisFile(filename):
			jukebox::factory::loadWaveFile(filename);

	printFileData(soundFile);

	/* 
	* the same 'soundFile' may be shared among several 'sounds',
	* each 'sound' holds its own playback context, independently 
	* from 'soundFile'.
	*/
	auto sound = jukebox::factory::makeSound(soundFile);
	sound.loop(true);

	jukebox::Mixer mixer;
	mixer.setVolume(80);
	sound.play();

	std::cout << "hit enter to exit..." << std::endl;
	std::cin.get();
}
```
# Credits

- MP3 decoder: https://github.com/lieff/minimp3
- Ogg Vorbis decoder: https://github.com/nothings/stb
- FluidSynth MIDI synthesizer: https://github.com/FluidSynth/fluidsynth
- Soundfont: http://schristiancollins.com/generaluser.php
