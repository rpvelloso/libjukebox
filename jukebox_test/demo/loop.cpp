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
#include <vector>
#include <algorithm>
#include "libjukebox.h"

// forward declarations
void printFileInfo(const jukebox::SoundFile &file);
jukebox::SoundFile loadSoundFile(const std::string &filename);

int main(int argc, char **argv) {
	if( argc < 3 ) {
		std::cout << "you need to supply two audio (wav/ogg/mp3/mid) files as arguments" << std::endl;
		return 1;
	}

	std::string filename1(argv[1]);
	std::string filename2(argv[2]);

	std::vector<jukebox::SoundFile> soundFiles;
	std::vector<jukebox::Sound> sounds;

	soundFiles.emplace_back(loadSoundFile(filename1));
	soundFiles.emplace_back(loadSoundFile(filename2));
	sounds.emplace_back(jukebox::factory::makeSound(soundFiles.front()));
	sounds.emplace_back(jukebox::factory::makeSound(soundFiles.back()));

	sounds.front().setOnStopCallback([&sounds](){
		std::cout << "sound 1 stopped. Starting sound 2..." << std::endl;
		sounds.back().play();
	});

	sounds.back().setOnStopCallback([&sounds](){
		std::cout << "sound 2 stopped. Starting sound 1..." << std::endl;
		sounds.front().play();
	});

	sounds.front().play(); // start the loop

	std::cout << "hit enter to exit..." << std::endl;
	std::cin.get();

	sounds.front().clearOnStopCallback(); // need to clear events in order to stop the loop
	sounds.front().stop();
	sounds.back().clearOnStopCallback();
	sounds.back().stop();
}

jukebox::SoundFile loadSoundFile(const std::string &filename) {
	auto extension = filename.substr(filename.find_last_of('.'));
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	if (extension == ".mid")
		return jukebox::factory::loadMIDIFile(filename);
	else
		return jukebox::factory::loadFile(filename);
}
