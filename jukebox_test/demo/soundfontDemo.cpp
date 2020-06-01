#include <fstream>
#include <iostream>
#include <string>
#include "jukebox/Decoders/MIDIConfigurator.h"
#include "jukebox/Sound/Factory.h"

#ifdef SOUNDFONT_PATH
#define xstr(a) str(a)
#define str(a) #a
std::string soundFontPath = xstr(SOUNDFONT_PATH);
#else
std::string soundFontPath = "";
#endif

int main(int argc, char **argv) {
	if( argc < 2 ) {
		std::cerr << "you need to supply a .mid file as an argument" << std::endl;
		return 1;
	}

	auto filename = std::string(argv[1]);
	std::cout << "ready to load " << filename << std::endl;
	auto soundFile = jukebox::factory::loadFile(filename);

	std::cout << "Playing MIDI with default sound font." << std::endl;
	auto sound = jukebox::factory::makeSound(soundFile);
	sound.play();
	sound.loop(true);

	std::cout << "type something + enter to change the soundfont" << std::endl;
	std::string dummy; std::getline(std::cin,dummy);

	std::ifstream fs(soundFontPath + "GeneralUser GS v1.471.sf2");
	if(!fs) {
		throw "Invalid sound font " + soundFontPath + "GeneralUser GS v1.471.sf2";
	}
	fs.close();

	std::cout << "Changing to GeneralUser GS" << std::endl;
	jukebox::MIDIConfigurator::getInstance().setSoundFont(soundFontPath + "GeneralUser GS v1.471.sf2");
	// TODO fix this bug that forces to use stop before reassigning it
	sound.stop();
	sound = jukebox::factory::makeSound(soundFile);
	sound.play();
	sound.loop(true);
	std::getline(std::cin,dummy);

	std::cout << "Changing to GXSCC" << std::endl;
	jukebox::MIDIConfigurator::getInstance().setSoundFont(soundFontPath + "GXSCC_gm_033.sf2");
	// TODO fix this bug that forces to use stop before reassigning it
	sound.stop();
	sound = jukebox::factory::makeSound(soundFile);
	sound.play();
	sound.loop(true);
	std::getline(std::cin,dummy);
}
