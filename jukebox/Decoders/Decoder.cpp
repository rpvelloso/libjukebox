/*
 * Decoder.cpp
 *
 *  Created on: 31 de jul de 2018
 *      Author: Benutzer
 */

#include "Decoder.h"

namespace jukebox {

Decoder::Decoder(DecoderImpl *impl) : impl(impl) {
}

int Decoder::getSamples(char* buf, int pos, int len) {
	return impl->getSamples(buf, pos, len);
}

} /* namespace socks */
