/*
 * DecoderImplDecorator.cpp
 *
 *  Created on: 7 de mar de 2019
 *      Author: roberto
 */

#include "DecoderImplDecorator.h"

namespace jukebox {
DecoderImplDecorator::DecoderImplDecorator(SoundFileImpl &fileImpl, DecoderImpl *impl) :
		DecoderImpl(fileImpl),
		impl(impl) {
}

int DecoderImplDecorator::getBlockSize() const {
	return impl->getBlockSize();
}

short DecoderImplDecorator::getNumChannels() const {
	return impl->getNumChannels();
}

int DecoderImplDecorator::getSampleRate() const {
	return impl->getSampleRate();
}

short DecoderImplDecorator::getBitsPerSample() const {
	return impl->getBitsPerSample();
}

int DecoderImplDecorator::getDataSize() const {
	return impl->getDataSize();
}

int DecoderImplDecorator::silenceLevel() const {
	return impl->silenceLevel();
}

DecoderImpl* DecoderImplDecorator::peel() {
	return impl.release();
}

}
