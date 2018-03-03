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

#include "SoundFile.h"
#include "SoundFileImpl.h"

namespace jukebox {

// TODO rename to WaveFileImpl?
class WaveFile : public SoundFileImpl {
	// TODO use fixed int types
	struct WaveHeader1 {
		char ChunkID[4];
		int ChunkSize;
		char Format[4];
		char Subchunk1ID[4];
		int Subchunk1Size;
	};
	struct WaveHeader2 {
		short AudioFormat;
		short NumChannels;
		int SampleRate;
		int ByteRate;
		short BlockAlign;
		short BitsPerSample;
	};
	struct WaveHeader3 {
		char Subchunk2ID[4];
		int Subchunk2Size;
	};
public:
	WaveFile(const std::string &filename);
	WaveFile(std::istream &inp, const std::string &filename = ":stream:");
	WaveFile(std::istream &&inp, const std::string &filename = ":stream:");
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	const char *getData() const override;
	int getDataSize() const override;
	const std::string &getFilename() const override;
private:
	std::string filename;
	WaveHeader1 header1;
	WaveHeader2 header2;
	WaveHeader3 header3;
	std::unique_ptr<char []> data;
};

namespace factory {
	SoundFile loadWaveFile(const std::string &filename);
	SoundFile loadWaveStream(std::istream &inp);
}

}

#endif /* WAVEFILE_H_ */