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
template<class T>
void printInfo(T &fileOrSound);

int main(int argc, char **argv) {
	if( argc < 2 ) {
		std::cout << "usage: " << argv[0] << " filename.[wav|ogg|mp3|mid|flac]" << std::endl;
		return 1;
	}

	std::string filename(argv[1]);

	try {
		// load a sound file
		//auto soundFile = jukebox::factory::loadFile(filename);

		// print FILE info
		//printInfo(soundFile);

		// create a sound from soundFile
		auto sound = jukebox::factory::makeSound(filename);

		/*
		 * print SOUND info, this may be different from FILE info
		 * (e.g., after resampling, changing sample resolution, etc)
		*/
		printInfo(sound);

		auto sound2 = sound.prototype();

		if (argc == 6) {
			auto delay = std::stof(argv[2]);
			auto decay = std::stof(argv[3]);
			auto numDelays = std::stoi(argv[4]);
			auto gain = std::stof(argv[5]);

			// add reverb and distortion
			sound
				.reverb(delay, decay, numDelays)
				.distortion(gain);
		}

		jukebox::Mixer mixer;
		mixer.setVolume(100); // max global volume

		sound
			.fadeOnStop(3) // 3s fade out on stop
			.loop(true) // looping
			.setVolume(100) // max sound volume
			.setOnStopCallback([](){std::cout << "parou!!!" << std::endl;}) // on stop event
			.addTimedEventCallback(2, [&sound](){
				std::cout << "adding effects!!!" << std::endl;
				sound
					.distortion(15)
					.reverb(0.01, 0.5, 3);
			}) // timed event: add effects
			.addTimedEventCallback(4, [&sound](){
				std::cout << "removing effects!!!" << std::endl;
				sound
					.peelDecoder()
					.peelDecoder();
			}) // timed event: remove last effects
			.play(); // start playing

		std::cout << "hit enter to fade out..." << std::endl;
		std::string dummy;
		std::getline(std::cin,dummy);
		std::cout << "fading out..." << std::endl;
		sound.stop(); // fade out the sound before stopping it
		sound2.play();

		std::cout << "hit enter to exit..." << std::endl;
		std::getline(std::cin,dummy);
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

template<class T>
void printInfo(T &fileOrSound) {
	static std::array<std::string, 2> channels = {"Mono", "Stereo"};

	std::cout << fileOrSound.getFilename() << " attributes: " << std::endl;
	std::cout << fileOrSound.getBitsPerSample() << " bits" << std::endl;
	std::cout << channels[fileOrSound.getNumChannels() - 1] << std::endl;
	std::cout << fileOrSound.getSampleRate() << " Hz" << std::endl;
	std::cout << fileOrSound.getDataSize() << " bytes" << std::endl;
	std::cout << formatDuration(fileOrSound.getDuration()) << " sec(s)" << std::endl << std::endl;
}
