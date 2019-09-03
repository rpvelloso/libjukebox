/*
 * DecoderImplDecorator.h
 *
 *  Created on: 7 de mar de 2019
 *      Author: roberto
 */

#ifndef JUKEBOX_DECODERS_DECORATORS_DECODERIMPLDECORATOR_H_
#define JUKEBOX_DECODERS_DECORATORS_DECODERIMPLDECORATOR_H_

#include <memory>
#include "../DecoderImpl.h"
#include "jukebox/FileFormats/SoundFileImpl.h"

namespace jukebox {
class DecoderImplDecorator: public DecoderImpl {
public:
	DecoderImplDecorator(SoundFileImpl &fileImpl, DecoderImpl *impl);
	virtual ~DecoderImplDecorator() = default;
	int getBlockSize() const override;
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	int getDataSize() const override;
	int silenceLevel() const override;
	DecoderImpl *peel() override;
protected:
	std::unique_ptr<DecoderImpl> impl;
};
}
#endif /* JUKEBOX_DECODERS_DECORATORS_DECODERIMPLDECORATOR_H_ */
