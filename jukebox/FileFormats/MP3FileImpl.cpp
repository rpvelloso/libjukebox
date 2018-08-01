/*
 * MP3FileImpl.cpp
 *
 *  Created on: 30 de jul de 2018
 *      Author: rvelloso
 */

#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <string.h>

#include "jukebox/Decoders/MP3DecoderImpl.h"
#include "SoundFile.h"
#include "Decorators/BufferedSoundFileImpl.h"
#include "Decorators/FadedSoundFileImpl.h"
#include "MP3FileImpl.h"

#define MINIMP3_IMPLEMENTATION
#include "jukebox/Decoders/minimp3/minimp3.h"

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
	return 16;
}

int MP3FileImpl::getDataSize() const {
	return dataSize;
}

const std::string& MP3FileImpl::getFilename() const {
	return filename;
}

int MP3FileImpl::read(char* buf, int pos, int len) { // unused
	throw std::runtime_error("invalid call to MP3FileImpl::read");
	return 0;
}

void MP3FileImpl::load(std::istream& inp) {
	auto fileStart = inp.tellg();
	inp.seekg(0, std::ios::end);
	fileSize = inp.tellg() - fileStart;
	inp.seekg(fileStart, std::ios::beg);

	fileBuffer.reset(new unsigned char[fileSize]);
	inp.read((char *)fileBuffer.get(), fileSize);

	mp3dec_t mp3d;
	mp3dec_init(&mp3d);

	int mp3_bytes = fileSize;
	uint8_t * buff = fileBuffer.get();
	int frameStart = 0;
	while (true) {
		int frame_size;
		auto i = mp3d_find_frame(buff, mp3_bytes, &(mp3d.free_format_bytes), &frame_size);
		if (!frame_size || i + frame_size > mp3_bytes)
			break;
		else {
			mp3_bytes -= (i + frame_size);
			buff += i;
		    numChannels = HDR_IS_MONO(buff) ? 1 : 2;
		    sampleRate = hdr_sample_rate_hz(buff);
			int frameSamples = hdr_frame_samples(buff) * numChannels*2;

		    if (frameSamples > 0) { // build frame index, used to seek
				//std::cout << (uint64_t)(buff - mp3.get()) << ": " << frameStart << ", " << (frameStart + frameSamples - 1) << std::endl;
				frameIndex.push_back(std::make_pair((frameStart + frameSamples - 1), buff - fileBuffer.get()));
				frameStart += frameSamples;

			    dataSize += frameSamples;
		    }

		    buff += frame_size;

		}
	}
}

std::unique_ptr<Decoder> MP3FileImpl::makeDecoder() {
	return std::make_unique<Decoder>(new MP3DecoderImpl(*this));
}

uint8_t* MP3FileImpl::getFileBuffer() {
	return fileBuffer.get();
}

int MP3FileImpl::getFileSize() const {
	return fileSize;
}

std::vector<std::pair<long, long>> &MP3FileImpl::getIndex() {
	return frameIndex;
}

namespace factory {
SoundFile loadMP3File(const std::string &filename) {
	return SoundFile(new MP3FileImpl(filename));
}

SoundFile loadMP3Stream(std::istream &inp) {
	return SoundFile(new MP3FileImpl(inp));
}

SoundFile loadBufferedMP3File(const std::string &filename) {
	return SoundFile(new BufferedSoundFileImpl(new MP3FileImpl(filename)));
}

SoundFile loadBufferedMP3Stream(std::istream &inp) {
	return SoundFile(new BufferedSoundFileImpl(new MP3FileImpl(inp)));
}

SoundFile loadFadedMP3File(const std::string &filename, int fadeInSecs, int fadeOutSecs) {
	return SoundFile(new FadedSoundFileImpl(new MP3FileImpl(filename), fadeInSecs, fadeOutSecs));
}

SoundFile loadFadedMP3Stream(std::istream &inp, int fadeInSecs, int fadeOutSecs) {
	return SoundFile(new FadedSoundFileImpl(new MP3FileImpl(inp), fadeInSecs, fadeOutSecs));
}

};

} /* namespace jukebox */
