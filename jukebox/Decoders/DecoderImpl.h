/*
 * DecoderImpl.h
 *
 *  Created on: 31 de jul de 2018
 *      Author: Benutzer
 */

#ifndef JUKEBOX_DECODERS_DECODERIMPL_H_
#define JUKEBOX_DECODERS_DECODERIMPL_H_

namespace jukebox {

class SoundFileImpl;

class DecoderImpl {
public:
	DecoderImpl(SoundFileImpl &fileImpl);
	virtual ~DecoderImpl() = default;
	virtual int getSamples(char *buf, int pos, int len) = 0;
	int getBlockSize() const;

private:
	int blockSize;
};

} /* namespace socks */

#endif /* JUKEBOX_DECODERS_DECODERIMPL_H_ */
