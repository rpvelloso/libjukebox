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

#include "SoundFileImpl.h"

namespace jukebox {

extern void freeVorbis(char *ptr);

class VorbisFile : public SoundFileImpl {
public:
	VorbisFile(const std::string &filename);
	VorbisFile(std::istream &inp);
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	const char *getData() const override;
	int getDataSize() const override;
	const std::string &getFilename() const;
private:
	short numChannels = 0;
	int sampleRate = 0;
	int dataSize = 0;
	std::unique_ptr<char, decltype(&freeVorbis)> data;
	std::string filename;

	void load(std::istream &inp);
};

} /* namespace jukebox */

#endif
