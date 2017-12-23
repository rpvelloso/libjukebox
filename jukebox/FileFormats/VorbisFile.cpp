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

#include <fstream>
#include <iostream>
#include <exception>
#include <stdlib.h>

#include "VorbisFile.h"
#include "SoundFile.h"
#include "jukebox/FileFormats/stb_vorbis/stb_vorbis.h"

namespace jukebox {

void freeVorbis(char* ptr) {
	free(ptr);
}

VorbisFile::VorbisFile(const std::string& filename) :
	SoundFileImpl(),
	data(nullptr, freeVorbis),
	filename(filename) {

	auto inp = std::fstream(this->filename, std::ios::binary|std::ios::in);
	load(inp);
}

VorbisFile::VorbisFile(std::istream& inp) :
	SoundFileImpl(),
	data(nullptr, freeVorbis),
	filename(":stream:") {

	load(inp);
}

void VorbisFile::load(std::istream& inp) {
	auto fileStart = inp.tellg();
	inp.seekg(0, std::ios::end);
	auto fileSize = inp.tellg() - fileStart;
	inp.seekg(fileStart, std::ios::beg);

	std::unique_ptr<unsigned char, decltype(&free)> buf((unsigned char *)malloc(fileSize), free);
	inp.read((char *)buf.get(), fileSize);

	int ch, srate, len;
	short *outp;

	len = stb_vorbis_decode_memory(buf.get(), fileSize, &ch, &srate, &outp);

	if (len == -1)
		throw std::runtime_error("stb_vorbis_decode_memory error");

	data.reset((char *)outp);
	numChannels = ch;
	sampleRate = srate;
	dataSize = len * numChannels * 2;
}

short VorbisFile::getNumChannels() const {
	return numChannels;
}

int VorbisFile::getSampleRate() const {
	return sampleRate;
}

short VorbisFile::getBitsPerSample() const {
	return 16;
}

const char* VorbisFile::getData() const {
	return data.get();
}

int VorbisFile::getDataSize() const {
	return dataSize;
}

const std::string& VorbisFile::getFilename() const {
	return filename;
}

namespace factory {
	SoundFile loadVorbisFile(const std::string &filename) {
		return SoundFile(new VorbisFile(filename));
	}

	SoundFile loadVorbisStream(std::istream &inp) {
		return SoundFile(new VorbisFile(inp));
	}
}

} /* namespace jukebox */

