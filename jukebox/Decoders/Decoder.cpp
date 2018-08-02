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
	if (len % impl->getBlockSize() != 0)
		throw std::runtime_error("invalid buffer size, should be block aligned.");
	if (pos % impl->getBlockSize() != 0)
		throw std::runtime_error("invalid stream position, should be block aligned.");

	return impl->getSamples(buf, pos, len);
}

} /* namespace socks */
