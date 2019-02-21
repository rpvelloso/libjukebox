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

#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <string.h>

#include "jukebox/Decoders/MP3DecoderImpl.h"
#include "SoundFile.h"
#include "MP3FileImpl.h"

#define DR_MP3_IMPLEMENTATION
#include "jukebox/Decoders/dr_mp3/dr_mp3.h"

namespace jukebox {

/* Memory loaders are useful for playing the same sound multiple times
 * and simultaneously. Their main use case is for small sound effects
 * (explosions, shots, etc)
 */
class MP3FileMemoryLoader : public MemoryFileLoader {
public:
	MP3FileMemoryLoader(SoundFileImpl &fileImpl, std::istream& inp) : MemoryFileLoader(fileImpl, inp) {}
	virtual ~MP3FileMemoryLoader() = default;

	void *createHandler() override {
		drmp3_config config = {
			(uint32_t)fileImpl.getNumChannels(),
			(uint32_t)fileImpl.getSampleRate()
		};

		std::unique_ptr<drmp3> ret(new drmp3);

		if (!drmp3_init_memory(ret.get(), (char *)memoryBuffer.get(), memoryBufferSize, &config))
			throw new std::runtime_error("error creating MP3 decoder handler from memory");

		return ret.release();
	};
};

/* Stream Loader is useful for large files, when it is not
 * practical to keep the entire encoded sound in memory.
 * The main use case is for background music. It is not possible
 * to play the same sound object simultaneously from a stream (you
 * need to create another sound object from the same file in order
 * to do this)
 */
class MP3FileStreamLoader : public FileLoader {
public:
	MP3FileStreamLoader(SoundFileImpl &fileImpl, std::istream& inp) : FileLoader(fileImpl, inp) {}
	virtual ~MP3FileStreamLoader() = default;

	void *createHandler() override {
		drmp3_config config = {
			(uint32_t)fileImpl.getNumChannels(),
			(uint32_t)fileImpl.getSampleRate()
		};

		inp.clear();
		inp.seekg(0, std::ios::beg);

		std::unique_ptr<drmp3> ret(new drmp3);
		if (!drmp3_init(
				ret.get(),
				(drmp3_read_proc)dr_libs_read_callback,
				(drmp3_seek_proc)dr_libs_seek_callback,
				(void *)&inp,
				&config)) {
			throw new std::runtime_error("error creating MP3 decoder handler from stream");
		}

		return ret.release();
	};
};

void closeMP3(drmp3 *f) {
	if (f) {
		drmp3_uninit(f);
		delete f; // dr_mp3 does not free the handler
	}
}

MP3FileImpl::MP3FileImpl(const std::string &filename, bool onMemory) :
	SoundFileImpl(),
	filename(filename),
	streamBuffer(new std::fstream(filename, std::ios::binary|std::ios::in)),
	inp(*streamBuffer),
	fileLoader(onMemory?
		(FileLoader *)new MP3FileMemoryLoader(*this, inp):
		(FileLoader *)new MP3FileStreamLoader(*this, inp)) {

	load();
}

MP3FileImpl::MP3FileImpl(std::istream& inp, bool onMemory) :
	SoundFileImpl(),
	filename(":stream:"),
	inp(inp),
	fileLoader(onMemory?
		(FileLoader *)new MP3FileMemoryLoader(*this, inp):
		(FileLoader *)new MP3FileStreamLoader(*this, inp)) {

	load();
}

short MP3FileImpl::getNumChannels() const {
	return numChannels;
}

int MP3FileImpl::getSampleRate() const {
	return sampleRate;
}

short MP3FileImpl::getBitsPerSample() const {
	return bitsPerSample;
}

const std::string& MP3FileImpl::getFilename() const {
	return filename;
}

void MP3FileImpl::load() {
	std::unique_ptr<drmp3, decltype(&closeMP3)> mp3(createHandler(), closeMP3);

	numChannels = mp3->channels;
	sampleRate = mp3->mp3FrameSampleRate;
	dataSize =
		drmp3_get_pcm_frame_count(mp3.get()) *
		numChannels *
		(bitsPerSample >> 3) *
		mp3->mp3FrameSampleRate / mp3->sampleRate;
}

DecoderImpl *MP3FileImpl::makeDecoder() {
	return new MP3DecoderImpl(*this);
}

drmp3 *MP3FileImpl::createHandler() {
	return (drmp3 *)fileLoader->createHandler();
}

} /* namespace jukebox */
