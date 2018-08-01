/*
 * WaveDecoder.cpp
 *
 *  Created on: 31 de jul de 2018
 *      Author: Benutzer
 */

#include "WaveDecoderImpl.h"

namespace jukebox {

WaveDecoderImpl::WaveDecoderImpl(WaveFileImpl& fileImpl) :
		DecoderImpl(), fileImpl(fileImpl) {
}

int WaveDecoderImpl::getSamples(char* buf, int pos, int len) {
	return fileImpl.read(buf, pos, len);
}

} /* namespace jukebox */
