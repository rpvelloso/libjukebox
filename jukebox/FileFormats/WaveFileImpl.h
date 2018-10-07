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
	struct WaveHeader1 {
		char ChunkID[4];
		uint32_t ChunkSize;
		char Format[4];
		char Subchunk1ID[4];
		uint32_t Subchunk1Size;
	};
	struct WaveHeader2 {
		uint16_t AudioFormat;
		uint16_t NumChannels;
		uint32_t SampleRate;
		uint32_t ByteRate;
		uint16_t BlockAlign;
		uint16_t BitsPerSample;
	};
	struct WaveHeader3 {
		char Subchunk2ID[4];
		uint32_t Subchunk2Size;
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
private:
	std::fstream fileStream;
	std::istream inputStream;
	std::string filename;
	WaveHeader1 header1;
	WaveHeader2 header2;
	WaveHeader3 header3;
	int headerSize = 0;
	std::mutex readMutex;

	void load();
};

}

#endif /* WAVEFILE_H_ */
