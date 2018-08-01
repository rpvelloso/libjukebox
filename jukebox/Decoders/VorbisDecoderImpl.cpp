/*
 * VorbisDecoderImpl.cpp
 *
 *  Created on: 31 de jul de 2018
 *      Author: Benutzer
 */

#include "VorbisDecoderImpl.h"
#include "jukebox/Decoders/stb_vorbis/stb_vorbis.c"

namespace jukebox {

VorbisDecoderImpl::VorbisDecoderImpl(VorbisFileImpl& fileImpl) :
	DecoderImpl(),
	fileImpl(fileImpl),
	numChannels(fileImpl.getNumChannels()),
	vorbisHandler(nullptr, closeVorbis) {

	int err;
	vorbisHandler.reset(stb_vorbis_open_memory(fileImpl.getFileBuffer(), fileImpl.getFileSize(), &err, nullptr));

	if (vorbisHandler.get() == nullptr)
		throw std::runtime_error("stb_vorbis_open_memory error: " + std::to_string(err));
}

int VorbisDecoderImpl::getSamples(char* buf, int pos, int len) {
	stb_vorbis_seek(vorbisHandler.get(), (pos / 2) / numChannels);
	return stb_vorbis_get_samples_short_interleaved(
		vorbisHandler.get(),
		numChannels,
		(short *)buf,
		len/2) * numChannels * 2;
}

}
