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
#include <fstream>
#include "jukebox/FileFormats/SoundFile.h"
#include "ModFileImpl.h"
#include "jukebox/Decoders/ModDecoderImpl.h"
extern "C" {
#include "jukebox/Decoders/mod/micromod.h"
}

namespace jukebox {

class ModFileMemoryLoader : public MemoryFileLoader {
public:
	ModFileMemoryLoader(SoundFileImpl &fileImpl, std::istream& inp) : MemoryFileLoader(fileImpl, inp) {}
	virtual ~ModFileMemoryLoader() = default;

	void *createHandler() override {
		return nullptr;
	};
};

ModFileImpl::ModFileImpl(const std::string &filename) :
	SoundFileImpl(),
	filename(filename) {

	auto inp = std::fstream(this->filename, std::ios::binary|std::ios::in);
	load(inp);
}

ModFileImpl::ModFileImpl(std::istream& inp) :
	SoundFileImpl(),
	filename(":stream:") {

	load(inp);
}

short ModFileImpl::getNumChannels() const {
	return 2;
}

int ModFileImpl::getSampleRate() const {
	return 44100;
}

short ModFileImpl::getBitsPerSample() const {
	return 16;
}

const std::string& ModFileImpl::getFilename() const {
	return filename;
}

DecoderImpl *ModFileImpl::makeDecoder() {
	return new ModDecoderImpl(*this);
}

void ModFileImpl::load(std::istream& inp) {
	fileLoader.reset(new ModFileMemoryLoader(*this, inp));

    struct micromod_obj mmodobj;
    auto result = micromod_initialise_obj(
        &mmodobj, 
        (signed char *)getMemoryBuffer(), 
        getSampleRate());

    if (result != 0) {
        throw std::runtime_error("Error loading mod file.");
    }

	dataSize = 
        micromod_calculate_song_duration_obj(&mmodobj) * // num of samples
		getNumChannels() * (getBitsPerSample()/8); // sample size
}

uint8_t* ModFileImpl::getMemoryBuffer() {
	return fileLoader->getMemoryBuffer();
}

int ModFileImpl::getBufferSize() {
	return fileLoader->getBufferSize();
}

} /* namespace jukebox */
