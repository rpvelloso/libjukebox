/*
 * WaveDecoder.h
 *
 *  Created on: 31 de jul de 2018
 *      Author: Benutzer
 */

#ifndef JUKEBOX_DECODERS_WAVEDECODERIMPL_H_
#define JUKEBOX_DECODERS_WAVEDECODERIMPL_H_

#include "jukebox/FileFormats/WaveFileImpl.h"
#include "DecoderImpl.h"

namespace jukebox {

class WaveDecoderImpl: public DecoderImpl {
public:
	WaveDecoderImpl(WaveFileImpl &fileImpl);
	virtual ~WaveDecoderImpl() = default;
	int getSamples(char *buf, int pos, int len) override;
private:
	WaveFileImpl &fileImpl;
};

} /* namespace jukebox */

#endif /* JUKEBOX_DECODERS_WAVEDECODERIMPL_H_ */
