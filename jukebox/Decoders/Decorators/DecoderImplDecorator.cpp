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

DecoderImpl* DecoderImplDecorator::peel() {
	return impl.release();
}

}
