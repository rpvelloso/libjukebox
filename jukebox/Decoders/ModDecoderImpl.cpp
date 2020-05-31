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

#include <cstring>
#include "jukebox/FileFormats/ModFileImpl.h"
#include "ModDecoderImpl.h"

namespace jukebox {

ModDecoderImpl::ModDecoderImpl(ModFileImpl& fileImpl) :
		DecoderImpl(fileImpl),
        sampleSize(fileImpl.getNumChannels() * (fileImpl.getBitsPerSample() / 8)),
		fileImpl(fileImpl) {
    auto result = 
        micromod_initialise_obj(
            &mmodobj, 
            (signed char *)fileImpl.getMemoryBuffer(), 
            fileImpl.getSampleRate());
    if (result != 0) {
        throw std::runtime_error("Error opening mod file in decoder.");
    }
}

int ModDecoderImpl::getSamples(char* buf, int pos, int len) {
    if (pos == 0)
        micromod_set_position_obj(&mmodobj, 0);

	if (pos >= fileImpl.getDataSize())
		return 0;

	memset(buf, 0, len);

	if (pos + len > fileImpl.getDataSize())
		len = fileImpl.getDataSize() - pos;

    micromod_get_audio_obj(&mmodobj, (int16_t *)buf, len/sampleSize);

	return len;
}

} /* namespace jukebox */
