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
#include "jukebox/Decoders/stb_vorbis/stb_vorbis.c"
#include "jukebox/Decoders/VorbisDecoderImpl.h"
#include "SoundFile.h"
#include "VorbisFileImpl.h"

namespace jukebox {

class VorbisFileMemoryLoader : public MemoryFileLoader {
public:
	VorbisFileMemoryLoader(SoundFileImpl &fileImpl, std::istream& inp) : MemoryFileLoader(fileImpl, inp) {}
	virtual ~VorbisFileMemoryLoader() = default;

	void *createHandler() override {
		int err;
		auto ret = stb_vorbis_open_memory(memoryBuffer.get(), memoryBufferSize, &err, nullptr);
		if (ret == nullptr)
			throw new std::runtime_error("error creating Vorbis decoder handler from memory");
		return (void *)ret;
	};
};

int stb_vorbis_fgetc_cb(void *h) {
	auto stream = (std::istream *)h;
	return stream->get();
}

int stb_vorbis_fseek_cb(void *h, long int off, int dir) {
	auto stream = (std::istream *)h;
	stream->seekg(off, (std::ios_base::seekdir)dir);
	return 0;
}

long int stb_vorbis_ftell_cb(void *h) {
	auto stream = (std::istream *)h;
	return stream->tellg();
}

size_t stb_vorbis_fread_cb(void *buf, size_t len, size_t count, void *h) {
	auto stream = (std::istream *)h;
	size_t ret = stream->read((char *)buf, len*count).gcount();
	return ret == (len*count)?count:0;
}

class VorbisFileStreamLoader : public FileLoader {
public:
	VorbisFileStreamLoader(SoundFileImpl &fileImpl, std::istream& inp) : FileLoader(fileImpl, inp) {}
	virtual ~VorbisFileStreamLoader() = default;

	void *createHandler() override {
		inp.clear();
		inp.seekg(0, std::ios::beg);

		int err;

		auto ret = stb_vorbis_open(
				(void *)&inp,
				&err,
				nullptr,
				stb_vorbis_fgetc_cb,
				stb_vorbis_fseek_cb,
				stb_vorbis_ftell_cb,
				stb_vorbis_fread_cb);

		if (ret == nullptr)
			throw new std::runtime_error("error creating Vorbis decoder handler from stream");

		return (void *)ret;
	};
};


void closeVorbis(stb_vorbis *v) {
	if (v)
		stb_vorbis_close(v);
}

VorbisFileImpl::VorbisFileImpl(const std::string& filename, bool onMemory) :
	SoundFileImpl(),
	filename(filename),
	streamBuffer(new std::fstream(filename, std::ios::binary|std::ios::in)),
	inp(*streamBuffer),
	fileLoader(onMemory?
		(FileLoader *)new VorbisFileMemoryLoader(*this, inp):
		(FileLoader *)new VorbisFileStreamLoader(*this, inp)) {

	load();
}

VorbisFileImpl::VorbisFileImpl(std::istream& inp, bool onMemory) :
	SoundFileImpl(),
	filename(":stream:"),
	inp(inp),
	fileLoader(onMemory?
		(FileLoader *)new VorbisFileMemoryLoader(*this, inp):
		(FileLoader *)new VorbisFileStreamLoader(*this, inp)) {

	load();
}

stb_vorbis* VorbisFileImpl::createHandler() {
	return (stb_vorbis *)fileLoader->createHandler();
}

void VorbisFileImpl::load() {
	std::unique_ptr<stb_vorbis, decltype(&closeVorbis)> vorbisHandler(createHandler(), closeVorbis);

	auto vorbisInfo = stb_vorbis_get_info(vorbisHandler.get());
	numChannels = vorbisInfo.channels;
	sampleRate = vorbisInfo.sample_rate;
	dataSize = stb_vorbis_stream_length_in_samples(vorbisHandler.get()) * numChannels * 2;
}

DecoderImpl *VorbisFileImpl::makeDecoder() {
	return new VorbisDecoderImpl(*this);
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

const std::string& VorbisFileImpl::getFilename() const {
	return filename;
}
} /* namespace jukebox */
