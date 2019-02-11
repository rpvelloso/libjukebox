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

#include <algorithm>
#include <exception>
#include <iostream>

#include "SoundFile.h"
#include "WaveFileImpl.h"
#include "../Decoders/dr_wav/dr_wav.h"
#include "../Decoders/WaveDecoderImpl.h"

namespace jukebox {

void closeWav(drwav *f) {
	if (f)
		drwav_close(f);
}


WaveFileImpl::WaveFileImpl(const std::string& filename) :
	SoundFileImpl(),
	filename(filename) {

	auto inp = std::fstream(this->filename, std::ios::binary|std::ios::in);
	load(inp);
}

WaveFileImpl::WaveFileImpl(std::istream &inp) :
	SoundFileImpl(),
	filename(":stream:") {

	load(inp);
}

short WaveFileImpl::getNumChannels() const {
	return numChannels;
}

int WaveFileImpl::getSampleRate() const {
	return sampleRate;
}

short WaveFileImpl::getBitsPerSample() const {
	return bitsPerSample;
}

DecoderImpl *WaveFileImpl::makeDecoder() {
	return new WaveDecoderImpl(*this);
}

void WaveFileImpl::load(std::istream &inp) {
	auto fileStart = inp.tellg();
	inp.seekg(0, std::ios::end);
	fileSize = inp.tellg() - fileStart;
	inp.seekg(fileStart, std::ios::beg);

	fileBuffer.reset(new uint8_t[fileSize]);
	inp.read((char *)fileBuffer.get(), fileSize);

	std::unique_ptr<drwav, decltype(&closeWav)> wavHandler(
		drwav_open_memory(fileBuffer.get(), fileSize), closeWav);

	if (wavHandler.get() == nullptr)
		throw std::runtime_error("drwav_open_memory error");

	numChannels = wavHandler->channels;
	sampleRate = wavHandler->sampleRate;
	bitsPerSample = wavHandler->bitsPerSample;
	dataSize = (wavHandler->totalPCMFrameCount * (bitsPerSample >> 3) * numChannels) ;
}

const std::string &WaveFileImpl::getFilename() const {
	return filename;
}

uint8_t* WaveFileImpl::getFileBuffer() {
	return fileBuffer.get();
}

int WaveFileImpl::getFileSize() const {
	return fileSize;
}

}
