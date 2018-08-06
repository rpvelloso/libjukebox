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

#ifndef LIBJUKEBOX_SOUNDFILEIMPL_2017_12_17_H_
#define LIBJUKEBOX_SOUNDFILEIMPL_2017_12_17_H_

#include <string>
#include "jukebox/Decoders/Decoder.h"

namespace jukebox {

class SoundFileImpl {
public:
	SoundFileImpl() = default;
	virtual ~SoundFileImpl() = default;
	virtual short getNumChannels() const = 0;
	virtual int getSampleRate() const = 0;
	virtual short getBitsPerSample() const = 0;
	virtual const std::string &getFilename() const = 0;
	virtual std::unique_ptr<Decoder> makeDecoder() = 0;
	virtual void truncAt(int pos);
	virtual int getDataSize() const;
protected:
	int dataSize = 0;
};

} /* namespace jukebox */

#endif /* SOUNDFILEIMPL_H_ */
