# libjukebox
Self-contained (i.e., no external dependencies) and platform independent (windows/linux) C++ audio playback library (uses DirectSound under Windows and Alsa under Linux).

# Supported file formats:
- WAVE (using [dr_wav](https://github.com/mackron/dr_libs));
- FLAC (using [dr_flac](https://github.com/mackron/dr_libs));
- MP3 (using [dr_mp3](https://github.com/mackron/dr_libs));
- Ogg Vorbis (using [stb_vorbis](https://github.com/nothings/stb));
- MIDI (using [FluiSynth](https://github.com/FluidSynth/fluidsynth) + [MidiFile](https://github.com/craigsapp/midifile));
- Mod (using [micromod fork](https://github.com/v1993/micromod));
- Extensible architecture allows implementation of other formats, transparently.

# Effects and features
- Fade in/out;
- Fade on stop;
- Simple reverberation (multiple delay lines with decay parameter);
- Distortion (tanh);
- File writer driver output;
- One shot timed events (with seconds resolution) and on stop event stack;
- Extensible architecture allows implementation of custom effects and audio drivers.

# Building

apt install libasound2-dev liblua5.3-dev
make -f makefile.linux
or
make -f makefile.mingw

This will compile the library (.so or .dll) and examples in 'bin/' directory. 
All you'll need to use is the header libjukebox.h and the library to link against your own project.

To provide MIDI support we are using [FluidSynth](https://github.com/FluidSynth/fluidsynth/wiki/BuildingWithCMake), but you don't need to install nor build another library prior to building libjukebox because FluidSynth already comes embedded into it (i.e., there is no external dependency with it).

# Simple Usage
```cpp
auto sound = jukebox::factory::makeSound("audio.mp3"); // create sound
sound.play(); // start playing
std::cout << "hit enter to stop..." << std::endl;
std::cin.get();

sound.stop(); // stop playing
std::cout << "hit enter to exit..." << std::endl;
std::cin.get();
```

# Combining effects
```cpp
auto sound = jukebox::factory::makeSound("audio.mp3"); // create sound

sound
	.fade(2, 2) // 2 seconds fade in & out
	.loop(true) // looping
	.setVolume(100) // 100% volume
	.distortion(50) // gain = 50
	.reverb(0.01, 0.8, 3) // robot voice (delay, decay, # delays)
	.play(); // start playing
	
std::cout << "hit enter to exit..." << std::endl;
std::cin.get();
```

# Example
```cpp
#include <iostream>
#include <algorithm>
#include <exception>
#include "libjukebox.h"

// forward declarations
void printFileInfo(const jukebox::SoundFile &file);

int main(int argc, char **argv) {
	if( argc < 2 ) {
		std::cout << "usage: " << argv[0] << " filename.[wav|ogg|mp3|mid|flac]" << std::endl;
		return 1;
	}

	std::string filename(argv[1]);

	try {
		// load a sound file
		auto soundFile = jukebox::factory::loadFile(filename);

		// print file info
		printFileInfo(soundFile);

		auto sound = jukebox::factory::makeSound(soundFile);

		jukebox::Mixer mixer;
		mixer.setVolume(100); // max global volume

		sound
			.fadeOnStop(3) // 3 seconds fade out on stop
			.loop(true) // looping
			.setVolume(100) // 100% volume 
			.play(); // start playing

		std::cout << "hit enter to fade out..." << std::endl;
		std::cin.get();
		sound.stop(); // fade out the sound before stopping it

		std::cout << "hit enter to exit..." << std::endl;
		std::cin.get();
	} catch (std::exception &e) {
		std::cerr << "error loading " << filename << ": " << e.what() << std::endl;
	}
	return 0;
}

std::string formatDuration(double duration) {
	int hr = duration/3600;
	duration -= hr*3600;
	int min = duration/60;
	duration -= min*60;
	int secs = duration;

	return
		std::to_string(hr) + ":" +
		(min<10?"0":"") + std::to_string(min) + ":" +
		(secs<10?"0":"") + std::to_string(secs);
};

void printFileInfo(const jukebox::SoundFile &file) {
	static std::array<std::string, 2> channels = {"Mono", "Stereo"};

	std::cout << file.getFilename() << " attributes: " << std::endl;
	std::cout << file.getBitsPerSample() << " bits" << std::endl;
	std::cout << channels[file.getNumChannels() - 1] << std::endl;
	std::cout << file.getSampleRate() << " Hz" << std::endl;
	std::cout << file.getDataSize() << " bytes" << std::endl;
	std::cout << formatDuration(file.getDuration()) << " sec(s)" << std::endl << std::endl;
}
```

# Credits

- Ogg Vorbis decoder: https://github.com/nothings/stb
- FluidSynth MIDI synthesizer: https://github.com/FluidSynth/fluidsynth
- Soundfont: http://schristiancollins.com/generaluser.php
- MIDI file parser: https://github.com/craigsapp/midifile
- MP3 decoder: https://github.com/lieff/minimp3
- MP3, FLAC & WAVE decoder: https://github.com/mackron/dr_libs
- Sol - C++ Lua Wrapper: https://github.com/ThePhD/sol2
- micromod - https://github.com/v1993/micromod, https://github.com/martincameron/micromod

# Collaborators

- [Diego](https://github.com/thinlizzy)
