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
#include "jukebox/Decoders/Decorators/JointStereoImpl.h"
#include "jukebox/Decoders/Decorators/MovingAverageImpl.h"
#include "Decorators/FadeOnStopSoundImpl.h"

namespace {

int normalize(int vol) { return std::max(0,std::min(vol,100)); }

}

namespace jukebox {

class DummyImpl: public SoundImpl {
	void play() override {}
	void stop() override {}
	void pause() override {}
	int getVolume() const override { return 0; }
	void setVolume(int) override {}
	void loop(bool) override {}
	void restart() override {}
	bool playing() const override { return false; }
	int getPosition() const override { return 0; }
	void setPosition(int pos) override {}
public:
	DummyImpl(): SoundImpl(0) {}
};

Sound::Sound() : impl(std::make_unique<DummyImpl>()) {
}

Sound::Sound(SoundImpl *impl) : impl(impl) {
}

Sound& Sound::play() {
	loop(looping);
	impl->play();
	return *this;
}

Sound& Sound::restart() {
	loop(looping);
	impl->restart();
	return *this;
}

Sound& Sound::pause() {
	impl->loop(false);
	impl->pause();
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

Sound& Sound::pushOnStopCallback(std::function<void(void)> os) {
	impl->pushOnStopCallback(os);
	return *this;
}

Sound& Sound::clearOnStopStack() {
	impl->clearOnStopStack();
	return *this;
}

std::function<void(void)> Sound::popOnStopCallback() {
	return impl->popOnStopCallback();
}

bool Sound::onStopStackEmpty() {
	return impl->onStopStackEmpty();
}

Sound &Sound::addTimedEventCallback(size_t seconds, std::function<void(void)> te) {
	impl->addTimedEventCallback(seconds, te);
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

/* changing sound properties (resolution, sample
 * rate, channels) is not allowed while playing
 */
Sound& Sound::resolution(int bitsPerSample) {
	if (!playing()) {
		impl->getDecoder().wrap<SampleResolutionImpl>(bitsPerSample);
	} // TODO: throw?
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

Sound& Sound::stop() {
	impl->loop(false);
	impl->stop();
	return *this;
}

Sound& Sound::jointStereo() {
	if (getNumChannels() == 2 && !playing()) {
		impl->getDecoder().wrap<JointStereoImpl>();
	} // TODO: throw?
	return *this;
}

Sound& Sound::movingAverage(float len) {
	impl->getDecoder().wrap<MovingAverageImpl>(len);
	return *this;
}

double Sound::getDuration() const {
	return impl->getDecoder().getDuration();
}

} /* namespace jukebox */
