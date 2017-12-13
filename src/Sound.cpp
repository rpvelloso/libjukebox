/*
 * Sound.cpp
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#include "Sound.h"

namespace jukebox {

Sound::Sound(SoundImpl *impl) : impl(impl) {
}

void Sound::play() {
	impl->play();
}

void Sound::stop() {
	impl->stop();
}

int Sound::getVolume() {
	return impl->getVolume();
}

void Sound::setVolume(int vol) {
	vol = std::min(vol, 100);
	vol = std::max(vol, 0);

	impl->setVolume(vol);
}

} /* namespace jukebox */
