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

#ifndef JUKEBOX_FILEFORMATS_MP3FILEIMPL_H_
#define JUKEBOX_FILEFORMATS_MP3FILEIMPL_H_

#include <string>
#include <memory>
#include <vector>
#include "SoundFileImpl.h"
#include "FileLoader.h"
#include "jukebox/Decoders/Decoder.h"
#include "jukebox/Decoders/dr_mp3/dr_mp3.h"

namespace jukebox {

extern void closeMP3(drmp3 *);

class MP3FileImpl : public SoundFileImpl {
public:
	MP3FileImpl(const std::string &filename);
	MP3FileImpl(std::istream& inp);
	virtual ~MP3FileImpl() = default;
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	const std::string &getFilename() const override;
	DecoderImpl *makeDecoder() override;
	drmp3 *createHandler();
private:
	std::string filename;
	int sampleRate = 0;
	short numChannels = 0;
	short bitsPerSample = 16;
	std::unique_ptr<std::istream> streamBuffer;
	std::istream &inp;
	std::unique_ptr<FileLoader> fileLoader;

	void load();
};

} /* namespace jukebox */

#endif /* JUKEBOX_FILEFORMATS_MP3FILEIMPL_H_ */
