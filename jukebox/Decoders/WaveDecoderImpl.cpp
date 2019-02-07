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

#include "WaveDecoderImpl.h"
#define DR_WAV_IMPLEMENTATION
#include "jukebox/Decoders/dr_wav/dr_wav.h"

#include <iostream>

namespace jukebox {

WaveDecoderImpl::WaveDecoderImpl(WaveFileImpl& fileImpl) :
		DecoderImpl(fileImpl),
		fileImpl(fileImpl),
		frameSize(fileImpl.getNumChannels() * (fileImpl.getBitsPerSample() >> 3)),
		wavHandler(nullptr, closeWav) {

	wavHandler.reset(drwav_open_memory(fileImpl.getFileBuffer(), fileImpl.getFileSize()));

	if (wavHandler.get() == nullptr)
		throw std::runtime_error("drwav_open_memory error");
}

short WaveDecoderImpl::getBitsPerSample() const {
	if (wavHandler->translatedFormatTag == DR_WAVE_FORMAT_PCM)
		return fileImpl.getBitsPerSample();
	else
		return fileImpl.getBitsPerSample() >= 32?32:16;
}

int WaveDecoderImpl::getSamples(char* buf, int pos, int len) {
	auto currentFrame = pos/frameSize;
	drwav_seek_to_pcm_frame(wavHandler.get(), currentFrame);

	auto numFrames = len/frameSize;

	if (wavHandler->translatedFormatTag == DR_WAVE_FORMAT_PCM)
		return drwav_read_pcm_frames(
			wavHandler.get(),
			numFrames,
			(int *)buf) * frameSize;
	else {
		if (fileImpl.getBitsPerSample() >= 32)
			return drwav_read_pcm_frames_s32(
				wavHandler.get(),
				numFrames,
				(int32_t *)buf) * frameSize;
		else
			return drwav_read_pcm_frames_s16(
				wavHandler.get(),
				numFrames,
				(int16_t *)buf) * frameSize;
	}
}

} /* namespace jukebox */
