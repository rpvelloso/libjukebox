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
#include "jukebox/Decoders/Decoder.h"

namespace jukebox {

class MP3FileImpl : public SoundFileImpl {
public:
	MP3FileImpl(const std::string &filename);
	MP3FileImpl(std::istream& inp);
	virtual ~MP3FileImpl();
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	const std::string &getFilename() const override;
	std::unique_ptr<Decoder> makeDecoder() override;
	uint8_t *getFileBuffer();
	int getFileSize() const;
	std::vector<std::pair<long, long>> &getIndex();
private:
	std::string filename;
	int sampleRate;
	short numChannels;
	std::unique_ptr<uint8_t> fileBuffer;
	int fileSize = 0;

	std::vector<std::pair<long, long> > frameIndex; // <endPos, FramePos>
	void load(std::istream& inp);
};

} /* namespace jukebox */

#endif /* JUKEBOX_FILEFORMATS_MP3FILEIMPL_H_ */
