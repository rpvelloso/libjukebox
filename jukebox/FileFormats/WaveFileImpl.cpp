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

class WaveFileMemoryLoader : public MemoryFileLoader {
public:
	WaveFileMemoryLoader(SoundFileImpl &fileImpl, std::istream& inp) : MemoryFileLoader(fileImpl, inp) {}
	virtual ~WaveFileMemoryLoader() = default;

	void *createHandler() override {
		auto ret = drwav_open_memory(memoryBuffer.get(), memoryBufferSize);
		if (ret == nullptr)
			throw new std::runtime_error("error creating WAV decoder handler from memory");

		return ret;
	};
};

class WaveFileStreamLoader : public FileLoader {
public:
	WaveFileStreamLoader(SoundFileImpl &fileImpl, std::istream& inp) : FileLoader(fileImpl, inp) {}
	virtual ~WaveFileStreamLoader() = default;

	void *createHandler() override {
		inp.clear();
		inp.seekg(0, std::ios::beg);

		auto ret = drwav_open(
				(drwav_read_proc)dr_libs_read_callback,
				(drwav_seek_proc)dr_libs_seek_callback,
				(void *)&inp);

		if (ret == nullptr)
			throw new std::runtime_error("error creating WAV decoder handler from stream");

		return ret;
	};
};

void closeWav(drwav *f) {
	if (f)
		drwav_close(f);
}


WaveFileImpl::WaveFileImpl(const std::string& filename, bool onMemory) :
	SoundFileImpl(),
	filename(filename),
	streamBuffer(new std::fstream(this->filename, std::ios::binary|std::ios::in)),
	inp(*streamBuffer),
	fileLoader(onMemory?
		(FileLoader *)new WaveFileMemoryLoader(*this, inp):
		(FileLoader *)new WaveFileStreamLoader(*this, inp)) {

	load();
}

WaveFileImpl::WaveFileImpl(std::istream &inp, bool onMemory) :
	SoundFileImpl(),
	filename(":stream:"),
	inp(inp),
	fileLoader(onMemory?
		(FileLoader *)new WaveFileMemoryLoader(*this, inp):
		(FileLoader *)new WaveFileStreamLoader(*this, inp)) {

	load();
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

const std::string &WaveFileImpl::getFilename() const {
	return filename;
}

DecoderImpl *WaveFileImpl::makeDecoder() {
	return new WaveDecoderImpl(*this);
}

drwav* WaveFileImpl::createHandler() {
	return (drwav *)fileLoader->createHandler();
}

void WaveFileImpl::load() {
	std::unique_ptr<drwav, decltype(&closeWav)> wavHandler(createHandler(), closeWav);

	numChannels = wavHandler->channels;
	sampleRate = wavHandler->sampleRate;
	bitsPerSample = wavHandler->bitsPerSample;
	dataSize = (wavHandler->totalPCMFrameCount * (bitsPerSample >> 3) * numChannels) ;
}

}
