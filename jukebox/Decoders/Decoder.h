/*
 * Decoder.h
 *
 *  Created on: 31 de jul de 2018
 *      Author: Benutzer
 */

#ifndef JUKEBOX_DECODERS_DECODER_H_
#define JUKEBOX_DECODERS_DECODER_H_

#include <memory>
#include "DecoderImpl.h"

namespace jukebox {

class Decoder {
public:
	Decoder(DecoderImpl * impl);
	int getSamples(char *buf, int pos, int len);
private:
	std::unique_ptr<DecoderImpl> impl;
};

} /* namespace socks */

#endif /* JUKEBOX_DECODERS_DECODER_H_ */
