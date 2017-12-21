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
#include <fstream>
#include <iostream>
#include <string>

#include "jukebox/FileFormats/SoundFile.h"
#include "jukebox/Mixer/Mixer.h"
#include "jukebox/Sound/Sound.h"

std::string formatDuration(double duration) {
	std::cout << "*** " << duration << std::endl;
	size_t hr = duration/3600;
	duration -= hr*3600;
	size_t min = duration/60;
	duration -= min*60;
	size_t secs = duration;

	return
		std::to_string(hr) + ":" +
		(min<10?"0":"") + std::to_string(min) + ":" +
		(secs<10?"0":"") + std::to_string(secs);
}

std::array<std::string, 2> channels = {"Mono", "Stereo"};

void printFileData(const jukebox::SoundFile &file) {
	std::cout << file.getFilename() << " attributes: " << std::endl;
	std::cout << file.getBitsPerSample() << " bits" << std::endl;
	std::cout << channels[file.getNumChannels() - 1] << std::endl;
	std::cout << file.getSampleRate() << " Hz" << std::endl;
	std::cout << file.getDataSize() << " bytes" << std::endl;
	std::cout << formatDuration(file.getDuration()) << std::endl << std::endl;
}

int main(int argc, char **argv) {
	if( argc < 3 ) {
		std::cout << "you need to supply two wave files as arguments" << std::endl;
		return 1;
	}

	std::cout << "ready to load " << argv[1] << " as a file" << std::endl;
	auto wav = jukebox::factory::loadWaveFile(argv[1]);
	printFileData(wav);

	std::cout << "ready to load " << argv[2] << " as a stream" << std::endl;
	std::fstream file(argv[2], std::ios::binary|std::ios::in);
	auto wav2 = jukebox::factory::loadWaveStream(file);
	printFileData(wav2);

	auto sound = jukebox::factory::makeSound(wav);
	auto sound2 = jukebox::factory::makeSound(wav2);

	jukebox::mixer.setVolume(100);
	std::cout << "vol: " << jukebox::mixer.getVolume() << std::endl;
	//sound.setVolume(100);
	sound.play();
	char n;
	std::cin >> n;
	sound.stop();
	jukebox::mixer.setVolume(10);
	std::cout << "vol: " << jukebox::mixer.getVolume() << std::endl;
	//sound.setVolume(30);
	sound.play();
	std::cin >> n;
	//sound2.setVolume(100);
	jukebox::mixer.setVolume(80);
	std::cout << "vol: " << jukebox::mixer.getVolume() << std::endl;
	sound2.play();
	std::cin >> n;

	return 0;
}
