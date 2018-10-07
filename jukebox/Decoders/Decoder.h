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

#include <memory>
#include "DecoderImpl.h"

namespace jukebox {

class Decoder {
public:
	Decoder(DecoderImpl * impl);
	int getSamples(char *buf, int pos, int len);
private:
	std::unique_ptr<DecoderImpl> impl;
};

} /* namespace socks */

#endif /* JUKEBOX_DECODERS_DECODER_H_ */
