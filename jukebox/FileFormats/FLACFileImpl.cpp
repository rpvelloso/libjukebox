/*
 * FLACFileImpl.cpp
 *
 *  Created on: 25 de jan de 2019
 *      Author: rvelloso
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
	return 16;
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
	dataSize = (flacHandler->totalSampleCount * 2) ;
}

} /* namespace jukebox */
