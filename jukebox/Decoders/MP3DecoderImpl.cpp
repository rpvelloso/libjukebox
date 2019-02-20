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

#include <iostream>
#include <algorithm>
#include <limits>
#include "MP3DecoderImpl.h"

namespace jukebox {

MP3DecoderImpl::MP3DecoderImpl(MP3FileImpl& fileImpl) :
	DecoderImpl(fileImpl),
	fileImpl(fileImpl),
	frameSize(fileImpl.getNumChannels() * (fileImpl.getBitsPerSample() >> 3)),
	mp3(fileImpl.createHandler(), closeMP3) {
}

int MP3DecoderImpl::getSamples(char* buf, int pos, int len) {
	drmp3_seek_to_pcm_frame(mp3.get(), pos / frameSize);

	size_t numFrames = len/frameSize;
	std::unique_ptr<float []> floatBuf(new float[numFrames*fileImpl.getNumChannels()]);
	auto ret = drmp3_read_pcm_frames_f32(mp3.get(), numFrames, floatBuf.get());
	auto sampleOut = (int16_t *)buf;
	for (size_t i = 0; i < numFrames*fileImpl.getNumChannels(); ++i, ++sampleOut)
		*sampleOut = floatBuf[i] * std::numeric_limits<int16_t>::max();
	return ret * frameSize;
}

} /* namespace jukebox */
