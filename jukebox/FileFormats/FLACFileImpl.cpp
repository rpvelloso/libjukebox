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

class FLACFileMemoryLoader : public MemoryFileLoader {
public:
	FLACFileMemoryLoader(SoundFileImpl &fileImpl, std::istream& inp) : MemoryFileLoader(fileImpl, inp) {}
	virtual ~FLACFileMemoryLoader() = default;

	void *createHandler() override {
		auto ret = drflac_open_memory(memoryBuffer.get(), fileSize);
		if (ret == nullptr)
			throw new std::runtime_error("error creating FLAC decoder handler from memory");

		return ret;
	};
};

class FLACFileStreamLoader : public FileLoader {
public:
	FLACFileStreamLoader(SoundFileImpl &fileImpl, std::istream& inp) : FileLoader(fileImpl, inp) {}
	virtual ~FLACFileStreamLoader() = default;

	void *createHandler() override {
		inp.clear();
		inp.seekg(0, std::ios::beg);

		auto ret = drflac_open(
				(drflac_read_proc)dr_libs_read_callback,
				(drflac_seek_proc)dr_libs_seek_callback,
				(void *)&inp);

		if (ret == nullptr)
			throw new std::runtime_error("error creating FLAC decoder handler from stream");

		return ret;
	};
};

void closeFlac(drflac *f) {
	if (f)
		drflac_close(f);
}

FLACFileImpl::FLACFileImpl(const std::string& filename, bool onMemory) :
	SoundFileImpl(),
	filename(filename),
	streamBuffer(new std::fstream(this->filename, std::ios::binary|std::ios::in)),
	inp(*streamBuffer),
	fileLoader(onMemory?
		(FileLoader *)new FLACFileMemoryLoader(*this, inp):
		(FileLoader *)new FLACFileStreamLoader(*this, inp)) {

	load();
}

FLACFileImpl::FLACFileImpl(std::istream& inp, bool onMemory) : SoundFileImpl(),
	filename(":stream:"),
	inp(inp),
	fileLoader(onMemory?
		(FileLoader *)new FLACFileMemoryLoader(*this, inp):
		(FileLoader *)new FLACFileStreamLoader(*this, inp)) {

	load();
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

DecoderImpl *FLACFileImpl::makeDecoder() {
	return new FLACDecoderImpl(*this);
}

drflac *FLACFileImpl::createHandler() {
	return (drflac *)fileLoader->createHandler();
}

void FLACFileImpl::load() {
	std::unique_ptr<drflac, decltype(&closeFlac)> flacHandler(createHandler(), closeFlac);

	numChannels = flacHandler->channels;
	sampleRate = flacHandler->sampleRate;
	bitsPerSample = flacHandler->bitsPerSample <= 16?16:32;
	dataSize = (flacHandler->totalSampleCount * (bitsPerSample >> 3)) ;
}

} /* namespace jukebox */
