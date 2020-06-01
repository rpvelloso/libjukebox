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

#ifndef JUKEBOX_FILEFORMATS_MODFILEIMPL_H_
#define JUKEBOX_FILEFORMATS_MODFILEIMPL_H_

#include <memory>
#include <string>
#include "SoundFileImpl.h"
#include "FileLoader.h"

namespace jukebox {

class ModFileImpl: public SoundFileImpl {
public:
	ModFileImpl(const std::string &filename);
	ModFileImpl(std::istream& inp);
	virtual ~ModFileImpl() = default;
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	const std::string &getFilename() const override;
	DecoderImpl *makeDecoder() override;
	uint8_t *getMemoryBuffer();
	int getBufferSize();
private:
	std::string filename;
	std::unique_ptr<MemoryFileLoader> fileLoader;

	void load(std::istream& inp);
};

} /* namespace jukebox */

#endif /* JUKEBOX_FILEFORMATS_MODFILEIMPL_H_ */
