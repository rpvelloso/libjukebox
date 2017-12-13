/*
 * main.cpp
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#include <iostream>
#include <array>

#include "libjukebox.h"

void printFileData(const jukebox::SoundFile &file) {
	static std::array<std::string, 2> channels = {"Mono", "Stereo"};

	std::cout << file.getFilename() << " attributes: " << std::endl;
	std::cout << file.getBitsPerSample() << " bits" << std::endl;
	std::cout << channels[file.getNumChannels() - 1] << std::endl;
	std::cout << file.getSampleRate() << " Hz" << std::endl;
	std::cout << file.getDataSize() << " bytes" << std::endl << std::endl;
}

int main(int argc, char **argv) {
	auto wav = jukebox::factory::loadWaveFile(argv[1]);
	auto wav2 = jukebox::factory::loadWaveFile(argv[2]);

	printFileData(wav);
	printFileData(wav2);

	auto sound = jukebox::factory::makeSound(wav);
	auto sound2 = jukebox::factory::makeSound(wav2);

	sound.play();
	char n;
	std::cin >> n;
	sound2.play();
	std::cin >> n;
}
