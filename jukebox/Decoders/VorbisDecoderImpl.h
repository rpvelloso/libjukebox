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

#ifndef JUKEBOX_DECODERS_VORBISDECODERIMPL_H_
#define JUKEBOX_DECODERS_VORBISDECODERIMPL_H_

#include <memory>
#include "jukebox/FileFormats/VorbisFileImpl.h"
#include "DecoderImpl.h"

namespace jukebox {

class VorbisDecoderImpl: public DecoderImpl {
public:
	VorbisDecoderImpl(VorbisFileImpl &fileImpl);
	virtual ~VorbisDecoderImpl() = default;
	int getSamples(char *buf, int pos, int len) override;
private:
	VorbisFileImpl &fileImpl;
	int numChannels;
	std::unique_ptr<stb_vorbis, decltype(&closeVorbis)> vorbisHandler;
};

} /* namespace jukebox */

#endif /* JUKEBOX_DECODERS_VORBISDECODERIMPL_H_ */
