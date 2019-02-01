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

#include "ReverbSoundImpl.h"
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "jukebox/Sound/SoundTransformation.h"

namespace jukebox {

class Reverb : public SoundTransformation {
public:
	Reverb(SoundFile &sf, float delay, float decay, int numDelays) :
		SoundTransformation(sf),
		numDelays(numDelays),
		delayBuffer(numDelays),
		bufPos(numDelays, 0),
		delay(delay),
		decay(decay) {

		reverb = reverbFunc[soundFile.getBitsPerSample()];

		float delayInterval = 1.0/numDelays;
		for (auto &delayLine: delayBuffer) {
			delayLine.resize(
				delay * delayInterval *
				(float)(sf.getSampleRate() *
				(sf.getBitsPerSample() >> 3) *
				sf.getNumChannels()));
			delayInterval += 0.5;
		}
};

	void operator()(void *buf, int pos, int len) override {
		reverb(*this, buf, pos, len);
	};
private:
	int numDelays;
	std::vector<std::vector<float> > delayBuffer;
	std::vector<size_t> bufPos;
	std::function<void(Reverb&, void *, int, int)> reverb;
	static std::unordered_map<short, decltype(reverb)> reverbFunc;
	float delay, decay;

	template<typename T>
	static void _reverb(Reverb& self, void *buf, int pos, int len) {
		T *beginIt = reinterpret_cast<T *>(buf);
		T *endIt = beginIt + (len/sizeof(T));
		auto offset = self.soundFile.silenceLevel();

		std::for_each(beginIt, endIt, [&self, offset](T &sample) {
			float signedSample = sample - offset;

			float delaySum = 0;

			for (int i = 0; i < self.numDelays; ++i)
				delaySum += self.delayBuffer[i][self.bufPos[i]]; // sum all delay lines

			signedSample =
				(signedSample + (delaySum*self.decay)) / // add attenuated echos
				((float)1.0 + (float)(self.numDelays)*self.decay); // weighted average

			sample = signedSample + offset;

			for (int i = 0; i < self.numDelays; ++i) {
				self.delayBuffer[i][self.bufPos[i]] = signedSample; // save the echo for the next time round
				self.bufPos[i] = (self.bufPos[i] + 1) % self.delayBuffer[i].size(); // circular echo/delay buffer
			}
		});
	};
};

std::unordered_map<short, decltype(Reverb::reverb)> Reverb::reverbFunc = {
		{8 , &Reverb::_reverb<uint8_t>},
		{16, &Reverb::_reverb<int16_t>},
		{32, &Reverb::_reverb<int32_t>}
};

ReverbSoundImpl::ReverbSoundImpl(SoundImpl *impl, float delay, float decay, int numDelays) :
		SoundImpl(impl->getSoundFile()),
		impl(impl) {

	impl->setTransformationCallback(Reverb(impl->getSoundFile(), delay, decay, numDelays));
}

void ReverbSoundImpl::play() {
	impl->play();
}

void ReverbSoundImpl::stop() {
	impl->stop();
}

int ReverbSoundImpl::getVolume() const {
	return impl->getVolume();
}

void ReverbSoundImpl::setVolume(int vol) {
	impl->setVolume(vol);
}

void ReverbSoundImpl::loop(bool loop) {
	impl->loop(loop);
}

void ReverbSoundImpl::setOnStopCallback(
		std::function<void(void)> os) {
	impl->setOnStopCallback(os);
}

} /* namespace jukebox */

