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
	numChannels(fileImpl.getNumChannels()),
	flacHandler(nullptr, closeFlac) {

	flacHandler.reset(drflac_open_memory(fileImpl.getFileBuffer(), fileImpl.getFileSize()));

	if (flacHandler.get() == nullptr)
		throw std::runtime_error("drflac_open_memory error");
}

int FLACDecoderImpl::getSamples(char* buf, int pos, int len) {
	drflac_seek_to_pcm_frame(flacHandler.get(), (pos / 2) / numChannels);
	return drflac_read_pcm_frames_s16(
		flacHandler.get(),
		len/2/numChannels,
		(short *)buf) * numChannels * 2;
}

}
