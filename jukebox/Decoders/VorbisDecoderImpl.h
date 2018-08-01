/*
 * VorbisDecoderImpl.h
 *
 *  Created on: 31 de jul de 2018
 *      Author: Benutzer
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
