/*
 * FadeOnStopSoundImpl.cpp
 *
 *  Created on: 4 de fev de 2019
 *      Author: rvelloso
 */

#include "FadeOnStopSoundImpl.h"

#include "../../Decoders/Decorators/FadeOnStopImpl.h"

namespace jukebox {

FadeOnStopSoundImpl::FadeOnStopSoundImpl(SoundImpl *impl, int fadeOutSecs) :
		SoundImpl(nullptr),
		impl(impl),
		fadeOutSecs(fadeOutSecs) {
}

void FadeOnStopSoundImpl::play() {
	impl->play();
}

void FadeOnStopSoundImpl::stop() {
	impl->getDecoder().wrapDecoder<FadeOnStopImpl>(fadeOutSecs, impl->getPosition());
}

int FadeOnStopSoundImpl::getVolume() const {
	return impl->getVolume();
}

void FadeOnStopSoundImpl::setVolume(int vol) {
	impl->setVolume(vol);
}

void FadeOnStopSoundImpl::loop(bool l) {
	impl->loop(l);
}

} /* namespace jukebox */
