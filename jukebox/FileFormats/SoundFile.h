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

#ifndef LIBJUKEBOX_SOUNDFILE_2017_12_17_H_
#define LIBJUKEBOX_SOUNDFILE_2017_12_17_H_

#include <memory>

#include "SoundFileImpl.h"

namespace jukebox {

class SoundFile {
public:
	SoundFile(SoundFileImpl *impl);
	short getNumChannels() const;
	int getSampleRate() const;
	short getBitsPerSample() const;
	int getDataSize() const;
	const std::string &getFilename() const;
	double getDuration() const;
	int read(char* buf, int pos, int len);
	void truncAt(int pos);
private:
	std::unique_ptr<SoundFileImpl> impl;
	int blockSize, dataSize;
};

// TODO this is very confusing: a wave file factory in SoundFile.h - perhaps move this factory to there and create a wave file impl, instead?
namespace factory {
	extern SoundFile loadMP3File(const std::string &filename);
	extern SoundFile loadWaveFile(const std::string &filename);
	extern SoundFile loadWaveStream(std::istream &inp);
	extern SoundFile loadVorbisFile(const std::string &filename);
	extern SoundFile loadVorbisStream(std::istream &inp);
	extern SoundFile loadBufferedWaveFile(const std::string &filename);
	extern SoundFile loadBufferedWaveStream(std::istream &inp);
	extern SoundFile loadBufferedVorbisFile(const std::string &filename);
	extern SoundFile loadBufferedVorbisStream(std::istream &inp);
	extern SoundFile loadFadedWaveFile(const std::string &filename, int fadeInSecs, int fadeOutSecs);
	extern SoundFile loadFadedWaveStream(std::istream &inp, int fadeInSecs, int fadeOutSecs);
	extern SoundFile loadFadedVorbisFile(const std::string &filename, int fadeInSecs, int fadeOutSecs);
	extern SoundFile loadFadedVorbisStream(std::istream &inp, int fadeInSecs, int fadeOutSecs);
}

} /* namespace jukebox */

#endif /* SOUNDFILE_H_ */
