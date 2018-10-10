/*
    Copyright 2017 Roberto Panerai Velloso.
    This file is part of libjukebox.
    libjukebox is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libjukebox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libjukebox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <algorithm>
#include <exception>
#include "libjukebox.h"

// forward declarations
void printFileInfo(const jukebox::SoundFile &file);
jukebox::SoundFile loadSoundFile(const std::string &filename);

int main(int argc, char **argv) {
	if( argc < 2 ) {
		std::cout << "usage: " << argv[0] << " filename.[wav|ogg|mp3|mid]" << std::endl;
		return 1;
	}

	std::string filename(argv[1]);

	try {
		// load a sound file
		auto soundFile = loadSoundFile(filename);

		// print file info
		printFileInfo(soundFile);

		/* create a sound (decorated with fade-on-stop functionality)
		 *  using previously loaded sound file */
		auto sound = jukebox::factory::makeFadeOnStopSound(soundFile, 3); // 3 seconds of fade out
		sound.loop(true); // set looping

		jukebox::Mixer mixer;
		mixer.setVolume(100); // max global volume
		sound.setVolume(100); // max sound volume
		sound.setOnStopCallback([](){std::cout << "parou!!!" << std::endl;});
		sound.play(); // start playing

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

jukebox::SoundFile loadSoundFile(const std::string &filename) {
	auto extension = filename.substr(filename.find_last_of('.'));
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	if (extension == ".mid")
		return jukebox::factory::loadMIDIFile(filename);
	else
		return jukebox::factory::loadFile(filename);
}
