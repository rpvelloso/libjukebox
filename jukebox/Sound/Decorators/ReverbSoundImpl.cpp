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
#include "jukebox/Sound/SoundTransformation.h"

namespace jukebox {

class Reverb : public SoundTransformation {
public:
	Reverb(SoundFile &sf, float delay, float decay) :
		SoundTransformation(sf),
		delay(delay),
		decay(decay) {

		reverb = reverbFunc[soundFile.getBitsPerSample()];

		echoBuffer.resize(
			delay *
			(float)(sf.getSampleRate() *
			(sf.getBitsPerSample() >> 3) *
			sf.getNumChannels()));
	};

	void operator()(void *buf, int pos, int len) override {
		reverb(*this, buf, pos, len);
	};
private:
	std::vector<int32_t> echoBuffer;
	size_t bufPos = 0;
	std::function<void(Reverb&, void *, int, int)> reverb;
	static std::unordered_map<short, decltype(reverb)> reverbFunc;
	float delay, decay;

	template<typename T>
	static void _reverb(Reverb& self, void *buf, int pos, int len) {
		T *beginIt = reinterpret_cast<T *>(buf);
		T *endIt = beginIt + (len/sizeof(T));
		int offset = self.soundFile.silenceLevel();

		std::for_each(beginIt, endIt, [&self, offset](T &sample) {
			int32_t signedSample = sample - offset;
			signedSample =
					(float)((float)signedSample * (1.0 - self.decay)) + // the original sound (some range sacrificed for the echo)
					((float)(self.echoBuffer[self.bufPos] * self.decay)*0.5); // the echo attenuated
			self.echoBuffer[self.bufPos] = signedSample; // save the echo for the next time round
			sample = signedSample + offset;
			self.bufPos = (self.bufPos + 1) % self.echoBuffer.size(); // circular echo/delay buffer
		});
	};
};

std::unordered_map<short, decltype(Reverb::reverb)> Reverb::reverbFunc = {
		{8 , &Reverb::_reverb<uint8_t>},
		{16, &Reverb::_reverb<int16_t>},
		{32, &Reverb::_reverb<int32_t>}
};

ReverbSoundImpl::ReverbSoundImpl(SoundImpl *impl, float delay, float decay) :
		SoundImpl(impl->getSoundFile()),
		impl(impl) {

	impl->setTransformationCallback(Reverb(impl->getSoundFile(), delay, decay));
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

