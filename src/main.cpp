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
#include <array>
#include <fstream>
#include <unistd.h>

#include "libjukebox.h"
#include "WindowsMixer.h"

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

void printFileData(const jukebox::SoundFile &file) {
	static std::array<std::string, 2> channels = {"Mono", "Stereo"};

	std::cout << file.getFilename() << " attributes: " << std::endl;
	std::cout << file.getBitsPerSample() << " bits" << std::endl;
	std::cout << channels[file.getNumChannels() - 1] << std::endl;
	std::cout << file.getSampleRate() << " Hz" << std::endl;
	std::cout << file.getDataSize() << " bytes" << std::endl;
	std::cout << formatDuration(file.getDuration()) << std::endl << std::endl;
}

int main(int argc, char **argv) {
	auto wav = jukebox::factory::loadWaveFile(argv[1]);

	std::fstream file(argv[2], std::ios::binary|std::ios::in);

	auto wav2 = jukebox::factory::loadWaveStream(file);

	printFileData(wav);
	printFileData(wav2);

	auto sound = jukebox::factory::makeSound(wav);
	auto sound2 = jukebox::factory::makeSound(wav2);

	jukebox::mixer.master().setVolume(50);
	//sound.setVolume(100);
	sound.play();
	char n;
	std::cin >> n;
	sound.stop();
	jukebox::mixer.master().setVolume(10);
	//sound.setVolume(30);
	sound.play();
	std::cin >> n;
	//sound2.setVolume(100);
	jukebox::mixer.master().setVolume(80);
	sound2.play();
	std::cin >> n;
}
