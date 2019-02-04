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

#ifndef JUKEBOX_FILEFORMATS_FLACFILEIMPL_H_
#define JUKEBOX_FILEFORMATS_FLACFILEIMPL_H_

#include <memory>
#include "jukebox/Decoders/Decoder.h"
#include "SoundFileImpl.h"
#include "jukebox/Decoders/dr_flac/dr_flac.h"

namespace jukebox {

extern void closeFlac(drflac *);

class FLACFileImpl : public SoundFileImpl {
public:
	FLACFileImpl(const std::string &filename);
	FLACFileImpl(std::istream &inp);
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	const std::string &getFilename() const override;
	DecoderImpl *makeDecoder() override;
	uint8_t *getFileBuffer();
	int getFileSize() const;
private:
	short numChannels = 0;
	int sampleRate = 0;
	int fileSize = 0;
	short bitsPerSample = 0;
	std::unique_ptr<uint8_t []> fileBuffer;
	std::string filename;

	void load(std::istream &inp);
};

} /* namespace jukebox */

#endif /* JUKEBOX_FILEFORMATS_FLACFILEIMPL_H_ */
