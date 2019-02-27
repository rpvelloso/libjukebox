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

#include <algorithm>
#include "Sound.h"
#include "jukebox/Decoders/Decoder.h"
#include "../Decoders/Decorators/FadeImpl.h"
#include "../Decoders/Decorators/ReverbImpl.h"
#include "../Decoders/Decorators/SampleResolutionImpl.h"
#include "../Decoders/Decorators/DistortionImpl.h"
#include "Decorators/FadeOnStopSoundImpl.h"
#include "../Decoders/MIDIDecoderImpl.h"

namespace {

int normalize(int vol) { return std::max(0,std::min(vol,100)); }

}

namespace jukebox {

Sound::Sound(SoundImpl *impl) : impl(impl) {
}

Sound& Sound::play() {
	//impl->stop();
	impl->play();
	return *this;
}

Sound& Sound::stop() {
	impl->stop();
	return *this;
}

int Sound::getVolume() const {
	auto vol = impl->getVolume();
	return normalize(vol);
}

Sound& Sound::setVolume(int vol) {
	impl->setVolume(normalize(vol));
	return *this;
}

Sound& Sound::loop(bool l) {
	impl->loop(l);
	return *this;
}

int Sound::getPosition() const {
	return impl->getPosition();
}

Sound& Sound::setOnStopCallback(std::function<void(void)> os) {
	impl->setOnStopCallback(os);
	return *this;
}

Sound& Sound::clearOnStopCallback() {
	impl->setOnStopCallback([](){});
	return *this;
}

Sound& Sound::reverb(float delay, float decay, size_t numDelays) {
	impl->getDecoder().wrapDecoder<ReverbImpl>(delay, decay, numDelays);
	return *this;
}

Sound& Sound::distortion(float gain) {
	impl->getDecoder().wrapDecoder<DistortionImpl>(gain);
	return *this;
}

Sound& Sound::fade(int fadeInSecs, int fadeOutSecs) {
	impl->getDecoder().wrapDecoder<FadeImpl>(fadeInSecs, fadeOutSecs);
	return *this;
}

Sound& Sound::resolution(int bitsPerSample) {
	impl->getDecoder().wrapDecoder<SampleResolutionImpl>(bitsPerSample);
	return *this;
}

Sound& Sound::fadeOnStop(int fadeOutSecs) {
	impl.reset(new FadeOnStopSoundImpl(impl.release(), fadeOutSecs));
	return *this;
}

} /* namespace jukebox */
