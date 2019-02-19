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
#include <limits>
#include <algorithm>
#include <string.h>

#include "jukebox/Decoders/MP3DecoderImpl.h"
#include "SoundFile.h"
#include "MP3FileImpl.h"

#define DR_MP3_IMPLEMENTATION
#include "jukebox/Decoders/dr_mp3/dr_mp3.h"

namespace jukebox {

MP3FileImpl::MP3FileImpl(const std::string &filename) :
	SoundFileImpl(),
	filename(filename) {

	auto inp = std::fstream(this->filename, std::ios::binary|std::ios::in);
	load(inp);
}

MP3FileImpl::MP3FileImpl(std::istream& inp) :
	SoundFileImpl(),
	filename(":stream:") {

	load(inp);
}

MP3FileImpl::~MP3FileImpl() {
}

short MP3FileImpl::getNumChannels() const {
	return numChannels;
}

int MP3FileImpl::getSampleRate() const {
	return sampleRate;
}

short MP3FileImpl::getBitsPerSample() const {
	return bitsPerSample;
}

const std::string& MP3FileImpl::getFilename() const {
	return filename;
}

void MP3FileImpl::load(std::istream& inp) {
	auto fileStart = inp.tellg();
	inp.seekg(0, std::ios::end);
	fileSize = inp.tellg() - fileStart;
	inp.seekg(fileStart, std::ios::beg);

	fileBuffer.reset(new unsigned char[fileSize]);
	inp.read((char *)fileBuffer.get(), fileSize);

	drmp3 mp3;
	drmp3_init_memory(&mp3, (char *)fileBuffer.get(), fileSize, nullptr);
	numChannels = mp3.channels;
	sampleRate = mp3.mp3FrameSampleRate;
	dataSize = drmp3_get_pcm_frame_count(&mp3) * numChannels * (bitsPerSample >> 3) * mp3.mp3FrameSampleRate / mp3.sampleRate;
	drmp3_uninit(&mp3);
}

DecoderImpl *MP3FileImpl::makeDecoder() {
	return new MP3DecoderImpl(*this);
}

uint8_t* MP3FileImpl::getFileBuffer() {
	return fileBuffer.get();
}

int MP3FileImpl::getFileSize() const {
	return fileSize;
}
} /* namespace jukebox */
