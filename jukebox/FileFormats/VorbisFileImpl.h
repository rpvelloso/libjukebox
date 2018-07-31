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

#ifndef LIBJUKEBOX_VORBISFILE_2017_12_23_H_
#define LIBJUKEBOX_VORBISFILE_2017_12_23_H_

#include <memory>
#include <mutex>

#include "SoundFile.h"
#include "SoundFileImpl.h"

typedef struct stb_vorbis stb_vorbis;

namespace jukebox {

extern void closeVorbis(stb_vorbis *);

class VorbisFileImpl : public SoundFileImpl {
public:
	VorbisFileImpl(const std::string &filename);
	VorbisFileImpl(std::istream &inp);
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	int getDataSize() const override;
	const std::string &getFilename() const override;
	int read(char *buf, int pos, int len) override;
private:
	short numChannels = 0;
	int sampleRate = 0;
	int dataSize = 0;
	std::unique_ptr<stb_vorbis, decltype(&closeVorbis)> vorbisHandler;
	std::unique_ptr<unsigned char []> file;
	std::string filename;
	std::mutex readMutex;

	void load(std::istream &inp);
};

namespace factory {
	SoundFile loadVorbisFile(const std::string &filename);
	SoundFile loadVorbisStream(std::istream &inp);
	SoundFile loadBufferedVorbisFile(const std::string &filename);
	SoundFile loadBufferedVorbisStream(std::istream &inp);
	SoundFile loadFadedVorbisFile(const std::string &filename, int fadeInSecs, int fadeOutSecs);
	SoundFile loadFadedVorbisStream(std::istream &inp, int fadeInSecs, int fadeOutSecs);
}

} /* namespace jukebox */

#endif
