/*
 * MP3FileImpl.cpp
 *
 *  Created on: 30 de jul de 2018
 *      Author: rvelloso
 */

#include <iostream>
#include <fstream>
#include <string.h>

#define MINIMP3_IMPLEMENTATION
#include "MP3FileImpl.h"
#include "SoundFile.h"

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

	while ((len > bytesRead) && (samples > 0)) {
		if (pcmPos < samples) {
			int siz = std::min(samples - pcmPos, (len - bytesRead) / 2);
			memcpy(&buf[bytesRead], &pcm[pcmPos], siz * 2);
			pcmPos += siz;
			bytesRead += (siz * 2);
		}

		if (pcmPos >= samples) {
			offset += info.frame_bytes;
			pcmPos = 0;
			samples = mp3dec_decode_frame(&mp3d, mp3.get() + offset, fileSize - offset, pcm, &info);
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
	while (true) {
		int frame_size;
		auto i = mp3d_find_frame(buff, mp3_bytes, &(mp3d.free_format_bytes), &frame_size);
		if (!frame_size || i + frame_size > mp3_bytes)
			break;
		else {
			mp3_bytes -= (i + frame_size);
			buff += i;

		    info.frame_bytes = i + frame_size;
		    info.channels = HDR_IS_MONO(buff) ? 1 : 2;
		    info.hz = hdr_sample_rate_hz(buff);
		    info.layer = 4 - HDR_GET_LAYER(buff);
		    info.bitrate_kbps = hdr_bitrate_kbps(buff);

		    //std::cout << info.bitrate_kbps << " " << info.hz << " " << info.channels << " " << hdr_frame_samples(buff) << std::endl;
		    dataSize += hdr_frame_samples(buff);
		    buff += frame_size;
		}
	}
	dataSize *= info.channels*2;
	mp3dec_init(&mp3d);
	samples = mp3dec_decode_frame(&mp3d, mp3.get(), fileSize, pcm, &info);
}

namespace factory {
	SoundFile loadMP3File(const std::string &filename) {
		return SoundFile(new MP3FileImpl(filename));
	}
};
} /* namespace jukebox */
