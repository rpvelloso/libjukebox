
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
#include "States/AlsaStopped.h"

#ifndef ALSA_DEVICE
#define ALSA_DEVICE "sysdefault"
#endif

namespace jukebox {


void closeAlsaHandle(snd_pcm_t *handle) {
	if (handle != nullptr) {
		snd_pcm_drop(handle);
		snd_pcm_close(handle);
	}
}

// AlsaHandle

AlsaHandle::AlsaHandle(Decoder *decoder) :
			SoundImpl(decoder),
			state(new AlsaStopped(*this)),
			handlePtr(nullptr, closeAlsaHandle) {

	snd_pcm_t *handle;
	auto res = snd_pcm_open(&handle, ALSA_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
	if (res != 0)
		throw std::runtime_error("snd_pcm_open error.");

	handlePtr.reset(handle);
}

void AlsaHandle::play() {
	state->play();
}

void AlsaHandle::pause() {
	state->pause();
}

void AlsaHandle::stop() {
	state->stop();
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

bool AlsaHandle::isLooping() const {
	return looping;
}

snd_pcm_t *AlsaHandle::getHandle() const {
	return handlePtr.get();
}
namespace factory {

SoundImpl *makeSoundImpl(Decoder *decoder) {
	return new AlsaHandle(decoder);
}

}

} /* namespace jukebox */
