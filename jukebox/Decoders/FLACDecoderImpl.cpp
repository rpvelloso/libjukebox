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

#include "FLACDecoderImpl.h"
#define DR_FLAC_IMPLEMENTATION
#include "jukebox/Decoders/dr_flac/dr_flac.h"

namespace jukebox {

FLACDecoderImpl::FLACDecoderImpl(FLACFileImpl& fileImpl) :
	DecoderImpl(fileImpl),
	fileImpl(fileImpl),
	bytesPerSample(fileImpl.getBitsPerSample() >> 3),
	frameSize(fileImpl.getNumChannels() * bytesPerSample),
	flacHandler(fileImpl.createHandler(), closeFlac) {
}

int FLACDecoderImpl::getSamples(char* buf, int pos, int len) {
	auto currentFrame = pos/frameSize;
	drflac_seek_to_pcm_frame(flacHandler.get(), currentFrame);

	auto numFrames = len/frameSize;

	if (bytesPerSample == 4)
		return drflac_read_pcm_frames_s32(
			flacHandler.get(),
			numFrames,
			(int32_t *)buf) * frameSize;

	return drflac_read_pcm_frames_s16(
		flacHandler.get(),
		numFrames,
		(int16_t *)buf) * frameSize;
}

}
