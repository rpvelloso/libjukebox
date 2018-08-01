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

#define STB_VORBIS_HEADER_ONLY
#include "jukebox/FileFormats/stb_vorbis/stb_vorbis.c"
#include "jukebox/Decoders/VorbisDecoderImpl.h"
#include "Decorators/BufferedSoundFileImpl.h"
#include "Decorators/FadedSoundFileImpl.h"
#include "SoundFile.h"
#include "VorbisFileImpl.h"

namespace jukebox {

void closeVorbis(stb_vorbis *v) {
	if (v)
		stb_vorbis_close(v);
}

VorbisFileImpl::VorbisFileImpl(const std::string& filename) :
	SoundFileImpl(),
	//vorbisHandler(nullptr, closeVorbis),
	filename(filename) {

	auto inp = std::fstream(this->filename, std::ios::binary|std::ios::in);
	load(inp);
}

VorbisFileImpl::VorbisFileImpl(std::istream& inp) :
	SoundFileImpl(),
	//vorbisHandler(nullptr, closeVorbis),
	filename(":stream:") {

	load(inp);
}

int VorbisFileImpl::read(char *buf, int pos, int len) {
	throw std::runtime_error("invalid call to VorbisFileImpl::read");
	return 0; // unused;
}
/*int VorbisFileImpl::read(char *buf, int pos, int len) {
	std::lock_guard<std::mutex> lock(readMutex);

	stb_vorbis_seek(vorbisHandler.get(), (pos / 2) / numChannels);
	return stb_vorbis_get_samples_short_interleaved(
		vorbisHandler.get(),
		numChannels,
		(short *)buf,
		len/2) * numChannels * 2;
}*/

void VorbisFileImpl::load(std::istream& inp) {
	auto fileStart = inp.tellg();
	inp.seekg(0, std::ios::end);
	fileSize = inp.tellg() - fileStart;
	inp.seekg(fileStart, std::ios::beg);

	fileBuffer.reset(new unsigned char[fileSize]);
	inp.read((char *)fileBuffer.get(), fileSize);

	int err;

	std::unique_ptr<stb_vorbis, decltype(&closeVorbis)> vorbisHandler(
			stb_vorbis_open_memory(fileBuffer.get(), fileSize, &err, nullptr), closeVorbis);

	if (vorbisHandler.get() == nullptr)
		throw std::runtime_error("stb_vorbis_open_memory error: " + std::to_string(err));

	auto vorbisInfo = stb_vorbis_get_info(vorbisHandler.get());
	numChannels = vorbisInfo.channels;
	sampleRate = vorbisInfo.sample_rate;
	dataSize = stb_vorbis_stream_length_in_samples(vorbisHandler.get()) * numChannels * 2;
}

std::unique_ptr<Decoder> VorbisFileImpl::makeDecoder() {
	return std::make_unique<Decoder>(new VorbisDecoderImpl(*this));
}

short VorbisFileImpl::getNumChannels() const {
	return numChannels;
}

int VorbisFileImpl::getSampleRate() const {
	return sampleRate;
}

short VorbisFileImpl::getBitsPerSample() const {
	return 16;
}

int VorbisFileImpl::getDataSize() const {
	return dataSize;
}

const std::string& VorbisFileImpl::getFilename() const {
	return filename;
}

uint8_t* VorbisFileImpl::getFileBuffer() {
	return fileBuffer.get();
}

int VorbisFileImpl::getFileSize() const {
	return fileSize;
}

namespace factory {
	SoundFile loadVorbisFile(const std::string &filename) {
		return SoundFile(new VorbisFileImpl(filename));
	}

	SoundFile loadVorbisStream(std::istream &inp) {
		return SoundFile(new VorbisFileImpl(inp));
	}

	SoundFile loadBufferedVorbisFile(const std::string &filename) {
		return SoundFile(new BufferedSoundFileImpl(new VorbisFileImpl(filename)));
	}

	SoundFile loadBufferedVorbisStream(std::istream &inp) {
		return SoundFile(new BufferedSoundFileImpl(new VorbisFileImpl(inp)));
	}

	SoundFile loadFadedVorbisFile(const std::string &filename, int fadeInSecs, int fadeOutSecs) {
		return SoundFile(new FadedSoundFileImpl(new VorbisFileImpl(filename), fadeInSecs, fadeOutSecs));
	}

	SoundFile loadFadedVorbisStream(std::istream &inp, int fadeInSecs, int fadeOutSecs) {
		return SoundFile(new FadedSoundFileImpl(new VorbisFileImpl(inp), fadeInSecs, fadeOutSecs));
	}

}

} /* namespace jukebox */
