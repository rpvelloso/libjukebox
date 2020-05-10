/*
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

#include <windows.h>

#include "DirectSoundBuffer.h"
#include "States/DirectSoundStopped.h"

namespace jukebox {

DirectSoundBuffer::DirectSoundBuffer(Decoder *decoder) :
	SoundImpl(decoder),
	state(new DirectSoundStopped(*this)) {
}

DirectSoundBuffer::~DirectSoundBuffer() {
	pause();
}

void DirectSoundBuffer::loop(bool l) {
	looping = l;
}

void DirectSoundBuffer::play() {
	state->play();
}

void DirectSoundBuffer::pause() {
	state->pause();
}

void DirectSoundBuffer::stop() {
	state->stop();
}

bool DirectSoundBuffer::playing() const {
	return state->playing();
}

int DirectSoundBuffer::getVolume() const {
	return state->getVolume();
}

void DirectSoundBuffer::setVolume(int vol) {
	state->setVolume(vol);
}

bool DirectSoundBuffer::isLooping() const {
	return looping;
}

namespace factory {

SoundImpl *makeSoundImpl(Decoder *decoder) {
	return new DirectSoundBuffer(decoder);
}

}

} /* namespace jukebox */
