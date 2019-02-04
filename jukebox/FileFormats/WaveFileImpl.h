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

#ifndef LIBJUKEBOX_WAVEFILE_2017_12_17_H_
#define LIBJUKEBOX_WAVEFILE_2017_12_17_H_

#include <memory>
#include <istream>
#include <fstream>
#include <cstdint>
#include <mutex>

#include "SoundFileImpl.h"
#include "jukebox/Decoders/Decoder.h"

namespace jukebox {

class WaveFileImpl : public SoundFileImpl {
	struct WaveRIFFWAVEfmtHeader {
		char chunkID[4];
		uint32_t chunkSize;
		char format[4];
		char subChunkID[4];
		uint32_t subChunkSize;
	};
	struct WaveFormatHeader {
		uint16_t audioFormat;
		uint16_t numChannels;
		uint32_t sampleRate;
		uint32_t byteRate;
		uint16_t blockAlign;
		uint16_t bitsPerSample;
	};
	struct WaveFactHeader {
		char chunkID[4];
		uint32_t chunkSize;
		uint32_t numSamples;
	};
	struct WavePEAKHeader {
		char chunkID[4];
		char filler[20];
	};
	struct WaveDataHeader {
		char chunkID[4];
		uint32_t chunkSize;
	};
public:
	WaveFileImpl(const std::string &filename);
	WaveFileImpl(std::istream &inp, const std::string &filename = ":stream:");
	virtual ~WaveFileImpl() = default;
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	const std::string &getFilename() const override;
	int read(char *buf, int pos, int len);
	std::unique_ptr<Decoder> makeDecoder();
	uint16_t getAudioFormat() const;
private:
	std::fstream fileStream;
	std::istream inputStream;
	std::string filename;
	WaveRIFFWAVEfmtHeader waveRIFFWAVEfmtHeader;
	WaveFormatHeader waveFormatHeader;
	WaveFactHeader waveFactHeader;
	WavePEAKHeader wavePEAKHeader;
	WaveDataHeader waveDataHeader;
	std::mutex readMutex;

	void load();
};

}

#endif /* WAVEFILE_H_ */
