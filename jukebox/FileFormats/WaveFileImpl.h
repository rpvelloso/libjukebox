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
#include "FileLoader.h"
#include "jukebox/Decoders/Decoder.h"
#include "jukebox/Decoders/dr_wav/dr_wav.h"


namespace jukebox {

extern void closeWav(drwav *f);

class WaveFileImpl : public SoundFileImpl {
public:
	WaveFileImpl(const std::string &filename);
	WaveFileImpl(std::istream &inp);
	virtual ~WaveFileImpl() = default;
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	const std::string &getFilename() const override;
	DecoderImpl *makeDecoder() override;
	drwav *createHandler();
private:
	short numChannels = 0;
	int sampleRate = 0;
	int fileSize = 0;
	short bitsPerSample = 0;
	std::string filename;
	std::unique_ptr<std::istream> streamBuffer;
	std::istream &inp;
	std::unique_ptr<FileLoader> fileLoader;

	void load();
};

}

#endif /* WAVEFILE_H_ */
