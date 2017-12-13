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

} /* namespace jukebox */
