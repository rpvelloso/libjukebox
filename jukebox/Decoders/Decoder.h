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

#ifndef JUKEBOX_DECODERS_DECODER_H_
#define JUKEBOX_DECODERS_DECODER_H_

#include <iostream>
#include <memory>
#include "DecoderImpl.h"

namespace jukebox {

class SoundFile;

class Decoder {
public:
	Decoder(SoundFile &soundFile);
	Decoder(std::shared_ptr<SoundFile> soundFilePtr);
	int getSamples(char *buf, int pos, int len);
	short getNumChannels() const;
	int getSampleRate() const;
	short getBitsPerSample() const;
	int getDataSize() const;
	int silenceLevel() const;
	Decoder *clone();

	template<typename T, typename ...Params> // T's base class must derive from DecoderImpl
	Decoder &wrap(Params&&... params) { // decorates current decoder
		impl.reset(new T(impl.release(), std::forward<Params>(params)...));
        return *this;
	}

	Decoder &peel();
private:
	std::shared_ptr<SoundFile> soundFilePtr;
	SoundFile &soundFile;
	std::unique_ptr<DecoderImpl> impl;
};

} /* namespace socks */

#endif /* JUKEBOX_DECODERS_DECODER_H_ */
