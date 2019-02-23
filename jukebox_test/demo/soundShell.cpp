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
#include <algorithm>
#include <exception>
#include "libjukebox.h"
#include "sol.hpp"

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
		"stop", &jukebox::Sound::stop,
		"getVolume", &jukebox::Sound::getVolume,
		"setVolume", &jukebox::Sound::setVolume,
		"loop", &jukebox::Sound::loop);

	lua.new_usertype<jukebox::factory::SoundBuilder>("SoundBuilder",
		sol::constructors<jukebox::factory::SoundBuilder(jukebox::Sound &)>(),
		"reverb", &jukebox::factory::SoundBuilder::reverb,
		"distortion", &jukebox::factory::SoundBuilder::distortion,
		"fade", &jukebox::factory::SoundBuilder::fade,
		"resolution", &jukebox::factory::SoundBuilder::resolution,
		"fadeOnStop", &jukebox::factory::SoundBuilder::fadeOnStop,
		"loop", &jukebox::factory::SoundBuilder::loop,
		"setVolume", &jukebox::factory::SoundBuilder::setVolume);

	lua["loadSoundFile"] = &jukebox::factory::loadFile;
	lua["makeSound"] = &jukebox::factory::makeSound;
	lua["makeSoundOutputToFile"] = &jukebox::factory::makeSoundOutputToFile;
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
	}

	if (script.status() != sol::load_status::ok) {
		std::cout <<
			"Error: " <<
			lua_tostring(lua.lua_state(), -1) <<
			std::endl;
		lua_pop(lua.lua_state(), 1);
	}
}



