/*
    Copyright 2019 Roberto Panerai Velloso.
    This file is part of libjukebox.
    libjukebox is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libjukebox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libjukebox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include "FadeOnStopSoundImpl.h"

#include "jukebox/Decoders/Decorators/FadeOnStopImpl.h"

namespace jukebox {

FadeOnStopSoundImpl::FadeOnStopSoundImpl(SoundImpl *impl, int fadeOutSecs) :
		SoundImpl(nullptr),
		impl(impl),
		fadeOutSecs(fadeOutSecs) {
}

void FadeOnStopSoundImpl::play() {
	impl->play();
}

void FadeOnStopSoundImpl::pause() {
	impl->pause();
}

void FadeOnStopSoundImpl::stop() {
	impl->pushOnStopCallback([this](){
		impl->getDecoder().peel();
	});
	impl->getDecoder().wrap<FadeOnStopImpl>(fadeOutSecs, impl->getPosition());

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

bool FadeOnStopSoundImpl::playing() const {
	return impl->playing();
}

int FadeOnStopSoundImpl::getPosition() const {
	return impl->getPosition();
}

void FadeOnStopSoundImpl::setPosition(int pos) {
	impl->setPosition(pos);
}

void FadeOnStopSoundImpl::pushOnStopCallback(std::function<void(void)> os) {
	impl->pushOnStopCallback(os);
}

std::function<void(void)> FadeOnStopSoundImpl::popOnStopCallback() {
	return impl->popOnStopCallback();
}

void FadeOnStopSoundImpl::clearOnStopStack() {
	impl->clearOnStopStack();
}

void FadeOnStopSoundImpl::addTimedEventCallback(size_t seconds, std::function<void(void)> te) {
	impl->addTimedEventCallback(seconds, te);
}

Decoder &FadeOnStopSoundImpl::getDecoder() {
	return impl->getDecoder();
}

} /* namespace jukebox */
