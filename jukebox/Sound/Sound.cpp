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
#include "Factory.h"
#include "jukebox/Decoders/Decoder.h"
#include "jukebox/Decoders/Decorators/FadeImpl.h"
#include "jukebox/Decoders/Decorators/ReverbImpl.h"
#include "jukebox/Decoders/Decorators/SampleResolutionImpl.h"
#include "jukebox/Decoders/Decorators/DistortionImpl.h"
#include "Decorators/FadeOnStopSoundImpl.h"

namespace {

int normalize(int vol) { return std::max(0,std::min(vol,100)); }

}

namespace jukebox {

Sound::Sound(SoundImpl *impl) : impl(impl) {
}

Sound& Sound::play() {
	loop(looping);
	impl->play();
	return *this;
}

Sound& Sound::restart() {
	impl->stop();
	impl->setPosition(0);
	loop(looping);
	impl->play();
	return *this;
}

Sound& Sound::stop() {
	impl->loop(false);
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
	looping = l;
	impl->loop(looping);
	return *this;
}

int Sound::getPosition() const {
	return impl->getPosition();
}

Sound& Sound::setPosition(int pos) {
	impl->setPosition(pos);
	return *this;
}

bool Sound::playing() const {
	return impl->playing();
}

Sound& Sound::setOnStopCallback(std::function<void(void)> os) {
	impl->setOnStopCallback(os);
	return *this;
}

Sound &Sound::addTimedEventCallback(size_t seconds, std::function<void(void)> te) {
	impl->addTimedEventCallback(seconds, te);
	return *this;
}

Sound& Sound::clearOnStopCallback() {
	impl->setOnStopCallback([](){});
	return *this;
}

Sound& Sound::reverb(float delay, float decay, size_t numDelays) {
	impl->getDecoder().wrap<ReverbImpl>(delay, decay, numDelays);
	return *this;
}

Sound& Sound::distortion(float gain) {
	impl->getDecoder().wrap<DistortionImpl>(gain);
	return *this;
}

Sound& Sound::fade(int fadeInSecs, int fadeOutSecs) {
	impl->getDecoder().wrap<FadeImpl>(fadeInSecs, fadeOutSecs);
	return *this;
}

Sound& Sound::resolution(int bitsPerSample) {
	impl->getDecoder().wrap<SampleResolutionImpl>(bitsPerSample);
	return *this;
}

Sound& Sound::fadeOnStop(int fadeOutSecs) {
	impl.reset(new FadeOnStopSoundImpl(impl.release(), fadeOutSecs));
	return *this;
}

Sound& Sound::peelDecoder() {
	impl->getDecoder().peel();
	return *this;
}

Sound Sound::prototype() {
	return Sound(factory::makeSoundImpl(impl->getDecoder().prototype()));
}

short Sound::getNumChannels() const {
	return impl->getDecoder().getNumChannels();
}

int Sound::getSampleRate() const {
	return impl->getDecoder().getSampleRate();
}

short Sound::getBitsPerSample() const {
	return impl->getDecoder().getBitsPerSample();
}

int Sound::getDataSize() const {
	return impl->getDecoder().getDataSize();
}

const std::string& Sound::getFilename() const {
	return impl->getDecoder().getFilename();
}

double Sound::getDuration() const {
	return impl->getDecoder().getDuration();
}

} /* namespace jukebox */
