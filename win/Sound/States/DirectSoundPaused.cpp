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

#include "DirectSoundPaused.h"
#include "DirectSoundPlaying.h"
#include "DirectSoundStopped.h"

namespace jukebox {

DirectSoundPaused::DirectSoundPaused(DirectSoundState &state) : DirectSoundState(state) {
}

void DirectSoundPaused::play() {
	dsound.setState<DirectSoundPlaying>();
}

void DirectSoundPaused::pause() {
	return;
}

void DirectSoundPaused::stop() {
	dsound.setState<DirectSoundStopped>();
}

int DirectSoundPaused::getVolume() const {
	return volume;
}

void DirectSoundPaused::setVolume(int vol) {
	volume = vol;
}

bool DirectSoundPaused::playing() const {
	return false;
}

DWORD DirectSoundPaused::status() const {
	return 0;
}

} /* namespace jukebox */
