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

#include "SoundFile.h"
#include "Decorators/BufferedSoundFileImpl.h"
#include "Decorators/FadedSoundFileImpl.h"
#define MINIMP3_IMPLEMENTATION
#include "MP3FileImpl.h"

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
	return info.channels;
}

int MP3FileImpl::getSampleRate() const {
	return info.hz;
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

int MP3FileImpl::read(char* buf, int pos, int len) { // 'pos' is unused, not supported for this file format
	int bytesRead = 0;

	if (pos == 0)
		reset();
	else
		positionMP3Stream(pos);

	while ((len > bytesRead) && (samples > 0)) {
		if (pcmPos < samples) {
			int siz = std::min(samples - pcmPos, (len - bytesRead) / 2);
			memcpy(&buf[bytesRead], &pcm[pcmPos], siz * 2);
			pcmPos += siz;
			bytesRead += (siz * 2);
		}

		if (pcmPos >= samples) {
			pcmPos = 0;
			std::cout << offset << std::endl;
			samples = mp3dec_decode_frame(&mp3d, mp3.get() + offset, fileSize - offset, pcm, &info)*info.channels;
			offset += info.frame_bytes;
		}
	}

	return bytesRead;
}

void MP3FileImpl::load(std::istream& inp) {
	auto fileStart = inp.tellg();
	inp.seekg(0, std::ios::end);
	fileSize = inp.tellg() - fileStart;
	inp.seekg(fileStart, std::ios::beg);

	mp3.reset(new unsigned char[fileSize]);
	inp.read((char *)mp3.get(), fileSize);

	mp3dec_init(&mp3d);
	int mp3_bytes = fileSize;
	uint8_t * buff = mp3.get();
	int frameStart = 0;
	while (true) {
		int frame_size;
		auto i = mp3d_find_frame(buff, mp3_bytes, &(mp3d.free_format_bytes), &frame_size);
		if (!frame_size || i + frame_size > mp3_bytes)
			break;
		else {
			mp3_bytes -= (i + frame_size);
			buff += i;
			int frameSamples = hdr_frame_samples(buff) * info.channels*2;

		    info.frame_bytes = i + frame_size;
		    info.channels = HDR_IS_MONO(buff) ? 1 : 2;
		    info.hz = hdr_sample_rate_hz(buff);
		    info.layer = 4 - HDR_GET_LAYER(buff);
		    info.bitrate_kbps = hdr_bitrate_kbps(buff);

		    if (frameSamples > 0) { // build frame index, used to seek
				std::cout << (uint64_t)(buff - mp3.get()) << ": " << frameStart << ", " << (frameStart + frameSamples - 1) << std::endl;
				frameIndex.push_back(std::make_pair((frameStart + frameSamples - 1), buff - mp3.get()));
				frameStart += frameSamples;

			    dataSize += frameSamples;
		    } else
		    	std::cout << (uint64_t)(buff - mp3.get()) << std::endl;

		    buff += frame_size;

		}
	}

	/*reset();
	positionMP3Stream(13);
	positionMP3Stream(4608);
	positionMP3Stream(4609);
	positionMP3Stream(105983);
	positionMP3Stream(4819968 + 10);
	positionMP3Stream(4889087 - 2000);
	reset();*/
}

void MP3FileImpl::positionMP3Stream(int pos) {
	class comp {
	public:
		bool operator()(const std::pair<int,int> &a, const std::pair<int,int> &b) const { return a.first < b.first;};
	};
	auto frameIt = std::lower_bound(frameIndex.begin(), frameIndex.end(), std::make_pair(pos, 0), comp());

	if (frameIt != frameIndex.end()) {
		auto frameEndPos = frameIt->first;
		auto frameOffset = frameIt->second;

		if (frameOffset != offset) {
			offset = frameOffset;
			samples = mp3dec_decode_frame(&mp3d, mp3.get() + frameOffset, fileSize - frameOffset, pcm, &info)*info.channels;
		}
		pcmPos = (pos - (frameEndPos - ((samples*2)-1))) / 2;
		std::cout << pos << " " << frameOffset << " " << pcmPos << " " <<  samples << std::endl;
	}
}

void MP3FileImpl::reset() {
	pcmPos = 0;
	mp3dec_init(&mp3d);
	samples = mp3dec_decode_frame(&mp3d, mp3.get(), fileSize, pcm, &info)*info.channels;
	offset = info.frame_bytes;
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
