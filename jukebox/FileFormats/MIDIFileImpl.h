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

#ifndef JUKEBOX_FILEFORMATS_MIDIFILEIMPL_H_
#define JUKEBOX_FILEFORMATS_MIDIFILEIMPL_H_

#include <memory>
#include <string>
#include "SoundFileImpl.h"

namespace jukebox {

class MIDIFileImpl: public SoundFileImpl {
public:
	MIDIFileImpl(const std::string &filename);
	MIDIFileImpl(std::istream& inp);
	virtual ~MIDIFileImpl() = default;
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	const std::string &getFilename() const override;
	std::unique_ptr<Decoder> makeDecoder() override;
	uint8_t *getFileBuffer();
	int getFileSize();
private:
	std::string filename;
	int fileSize = 0;
	std::unique_ptr<uint8_t> fileBuffer;

	void load(std::istream& inp);
};

} /* namespace jukebox */

#endif /* JUKEBOX_FILEFORMATS_MIDIFILEIMPL_H_ */
