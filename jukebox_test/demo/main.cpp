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

#include <array>
#include <iostream>
#include <string>
#include "jukebox/Mixer/Mixer.h"
#include "jukebox/Sound/Factory.h"
#include "jukebox/Sound/Sound.h"

#ifdef SOUND_PATH
#define xstr(a) str(a)
#define str(a) #a
std::string soundPath = xstr(SOUND_PATH);
#else
std::string soundPath = "";
#endif

std::string formatDuration(double duration) {
	int hr = duration/3600;
	duration -= hr*3600;
	int min = duration/60;
	duration -= min*60;
	int secs = duration;

	return std::to_string(hr) + ":" +
		(min<10?"0":"") + std::to_string(min) + ":" +
		(secs<10?"0":"") + std::to_string(secs);
}

std::array<std::string, 2> channels = {"Mono", "Stereo"};

void printFileInfo(const jukebox::SoundFile &file) {
	std::cout << file.getFilename() << " attributes: " << std::endl;
	std::cout << file.getBitsPerSample() << " bits" << std::endl;
	std::cout << channels[file.getNumChannels() - 1] << std::endl;
	std::cout << file.getSampleRate() << " Hz" << std::endl;
	std::cout << file.getDataSize() << " bytes" << std::endl;
	std::cout << formatDuration(file.getDuration()) << std::endl << std::endl;
}

int main(int argc, char **argv) {
	if( argc < 2 ) {
		std::cout << "you need to supply an audio (wav/ogg/mp3/mid/flac/mod) file as an argument" << std::endl;
		return 1;
	}

	std::string filename1(argv[1]);
	filename1 = soundPath + filename1;

	std::cout << "ready to load " << filename1 << " as a file" << std::endl;

	auto soundFile1 = jukebox::factory::loadFile(filename1);
	printFileInfo(soundFile1);

	auto sound1 = jukebox::factory::makeSound(soundFile1);
	sound1.loop(true);

	jukebox::Mixer mixer;
	mixer.setVolume(100);
	std::cout << "mixer volume: " << mixer.getVolume() << std::endl;
	sound1.play();
	// TODO replace with a proper key reading
	std::string dummy;
	std::getline(std::cin,dummy);

	mixer.setVolume(40);
	std::cout << "mixer volume: " << mixer.getVolume() << std::endl;
	// TODO replace with a proper key reading
	std::getline(std::cin,dummy);

	return 0;
}
