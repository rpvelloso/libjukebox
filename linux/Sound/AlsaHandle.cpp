
/*
    Copyright 2017 Roberto Panerai Velloso.
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
#include "AlsaHandle.h"
#include "States/AlsaPaused.h"

namespace jukebox {

// AlsaHandle

AlsaHandle::AlsaHandle(Decoder *decoder) :
			SoundImpl(decoder),
			state(new AlsaPaused(*this)) {

}

void AlsaHandle::play() {
	state->play();
}

void AlsaHandle::pause() {
	state->pause();
}

AlsaHandle::~AlsaHandle() {
	pause();
};

void AlsaHandle::loop(bool l) {
	looping = l;
}

bool AlsaHandle::playing() const {
	return state->playing();
}

int AlsaHandle::getVolume() const {
	return state->getVolume();
}

void AlsaHandle::setVolume(int vol) {
	state->setVolume(vol);
}

void AlsaHandle::setState(AlsaState* newState) {
	state.reset(newState);
}

std::vector<std::function<void(void)> >& AlsaHandle::getOnStopStack() {
	return onStopStack;
}

bool AlsaHandle::isLooping() const {
	return looping;
}

namespace factory {

SoundImpl *makeSoundImpl(Decoder *decoder) {
	return new AlsaHandle(decoder);
}

}

} /* namespace jukebox */
