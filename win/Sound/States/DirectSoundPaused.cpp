/*
 * DirectSoundPaused.cpp
 *
 *  Created on: 9 de set de 2019
 *      Author: rvelloso
 */

#include "DirectSoundPaused.h"
#include "DirectSoundPlaying.h"

namespace jukebox {

DirectSoundPaused::DirectSoundPaused(DirectSoundBuffer &dsound) : DirectSoundState(dsound) {
}

} /* namespace jukebox */

void jukebox::DirectSoundPaused::play() {
	dsound.setState(new DirectSoundPlaying(dsound));
}

void jukebox::DirectSoundPaused::pause() {
	return;
}

int jukebox::DirectSoundPaused::getVolume() const {
	return 0;
}

void jukebox::DirectSoundPaused::setVolume(int vol) {
	return;
}

bool jukebox::DirectSoundPaused::playing() const {
	return false;
}

DWORD jukebox::DirectSoundPaused::status() const {
	return 0;
}
