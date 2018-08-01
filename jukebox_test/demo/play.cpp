/*
 * play.cpp
 *
 *  Created on: 26 de jul de 2018
 *      Author: rvelloso
 */

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
			jukebox::factory::loadVorbisFile(filename)://, 10, 10):
			filename.back() == '3'? // mp3?
			jukebox::factory::loadMP3File(filename):
			jukebox::factory::loadWaveFile(filename);//, 10, 10);

	printFileData(soundFile);

	auto sound = jukebox::factory::makeFadeOnStopSound(soundFile, 3);
	auto sound2 = jukebox::factory::makeSound(soundFile);
	sound.loop(true);

	jukebox::Mixer mixer;
	mixer.setVolume(100);
	sound.play();

	std::cout << "hit enter to fade out..." << std::endl;
	std::cin.get();
	sound2.play();

	sound.stop();

	std::cout << "hit enter to exit..." << std::endl;
	std::cin.get();
}
