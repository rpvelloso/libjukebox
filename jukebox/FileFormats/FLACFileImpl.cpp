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
#include <istream>
#include <exception>
#include <stdlib.h>

#include "jukebox/Decoders/FLACDecoderImpl.h"
#include "SoundFile.h"
#include "FLACFileImpl.h"

namespace jukebox {

void closeFlac(drflac *f) {
	if (f)
		drflac_close(f);
}

FLACFileImpl::FLACFileImpl(const std::string& filename) :
	SoundFileImpl(),
	filename(filename) {

	auto inp = std::fstream(this->filename, std::ios::binary|std::ios::in);
	load(inp);
}

FLACFileImpl::FLACFileImpl(std::istream& inp) :	SoundFileImpl(),
	filename(":stream:") {

	load(inp);
}

short FLACFileImpl::getNumChannels() const {
	return numChannels;
}

int FLACFileImpl::getSampleRate() const {
	return sampleRate;
}

short FLACFileImpl::getBitsPerSample() const {
	return bitsPerSample;
}

const std::string& FLACFileImpl::getFilename() const {
	return filename;
}

std::unique_ptr<Decoder> FLACFileImpl::makeDecoder() {
	return std::make_unique<Decoder>(new FLACDecoderImpl(*this));
}

uint8_t* FLACFileImpl::getFileBuffer() {
	return fileBuffer.get();
}

int FLACFileImpl::getFileSize() const {
	return fileSize;
}

void FLACFileImpl::load(std::istream& inp) {
	auto fileStart = inp.tellg();
	inp.seekg(0, std::ios::end);
	fileSize = inp.tellg() - fileStart;
	inp.seekg(fileStart, std::ios::beg);

	fileBuffer.reset(new unsigned char[fileSize]);
	inp.read((char *)fileBuffer.get(), fileSize);

	std::unique_ptr<drflac, decltype(&closeFlac)> flacHandler(
			drflac_open_memory(fileBuffer.get(), fileSize), closeFlac);

	if (flacHandler.get() == nullptr)
		throw std::runtime_error("drflac_open_memory error");

	numChannels = flacHandler->channels;
	sampleRate = flacHandler->sampleRate;
	bitsPerSample = flacHandler->bitsPerSample <= 16?16:32;
	dataSize = (flacHandler->totalSampleCount * (bitsPerSample >> 3)) ;
}

} /* namespace jukebox */
