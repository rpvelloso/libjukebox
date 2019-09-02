/*
    Copyright 2019 Roberto Panerai Velloso.
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
#include <exception>
#include "libjukebox.h"
#include "sol.hpp"

jukebox::Sound makeSound(jukebox::SoundFile &file) {
	return jukebox::factory::makeSound(file);
}

jukebox::Sound makeSoundOutputToFile(jukebox::SoundFile &file, const std::string &filename) {
	return jukebox::factory::makeSoundOutputToFile(file, filename);
}

void bind(sol::state &lua) {
	lua.new_usertype<jukebox::Mixer>("Mixer",
		"getVolume", &jukebox::Mixer::getVolume,
		"setVolume", &jukebox::Mixer::setVolume);

	lua.new_usertype<jukebox::SoundFile>("SoundFile",
		"getNumChannels", &jukebox::SoundFile::getNumChannels,
		"getSampleRate", &jukebox::SoundFile::getSampleRate,
		"getBitsPerSample", &jukebox::SoundFile::getBitsPerSample,
		"getDataSize", &jukebox::SoundFile::getDataSize,
		"getFilename", &jukebox::SoundFile::getFilename,
		"getDuration", &jukebox::SoundFile::getDuration);

	lua.new_usertype<jukebox::Sound>("Sound",
		"play", &jukebox::Sound::play,
		"restart", &jukebox::Sound::restart,
		"pause", &jukebox::Sound::pause,
		"stop", &jukebox::Sound::stop,
		"getVolume", &jukebox::Sound::getVolume,
		"setVolume", &jukebox::Sound::setVolume,
		"getPosition", &jukebox::Sound::getPosition,
		"setPosition", &jukebox::Sound::setPosition,
		"loop", &jukebox::Sound::loop,
		"peelDecoder", &jukebox::Sound::peelDecoder,
		"reverb", &jukebox::Sound::reverb,
		"distortion", &jukebox::Sound::distortion,
		"jointStereo", &jukebox::Sound::jointStereo,
		"fade", &jukebox::Sound::fade,
		"resolution", &jukebox::Sound::resolution,
		"fadeOnStop", &jukebox::Sound::fadeOnStop,
		"getNumChannels", &jukebox::Sound::getNumChannels,
		"getSampleRate", &jukebox::Sound::getSampleRate,
		"getBitsPerSample", &jukebox::Sound::getBitsPerSample,
		"getDataSize", &jukebox::Sound::getDataSize,
		"getFilename", &jukebox::Sound::getFilename,
		"getDuration", &jukebox::Sound::getDuration);

	lua.new_usertype<jukebox::MIDIConfigurator>("MIDIConfigurator",
		"setSoundFont", &jukebox::MIDIConfigurator::setSoundFont,
		"getSoundFont", &jukebox::MIDIConfigurator::getSoundFont);

	lua["loadSoundFile"] = &jukebox::factory::loadFile;
	lua["midiConfig"] = &jukebox::MIDIConfigurator::getInstance();
	lua["makeSound"] = &makeSound;
	lua["makeSoundOutputToFile"] = &makeSoundOutputToFile;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "usage: " << argv[0] << " script.lua" << std::endl;
		std::terminate();
	}

	sol::state lua;

	lua.open_libraries();
	auto script = lua.load_file(argv[1]);

	if (script.status() == sol::load_status::ok) {
		bind(lua);
		script();
	} else {
		std::cout <<
			"Error: " <<
			lua_tostring(lua.lua_state(), -1) <<
			std::endl;
		lua_pop(lua.lua_state(), 1);
	}
}



