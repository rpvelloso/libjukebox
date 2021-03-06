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

#include "ReverbImpl.h"

#include <iostream>

namespace jukebox {

std::unordered_map<short, decltype(ReverbImpl::reverb)> ReverbImpl::reverbFunc = {
		{ 8, ReverbImpl::_reverb<uint8_t>},
		{16, ReverbImpl::_reverb<int16_t>},
		{32, ReverbImpl::_reverb<int32_t>}
};

template<typename T>
void ReverbImpl::_reverb(ReverbImpl &self, void *buf, int pos, int len) {
	T *beginIt = reinterpret_cast<T *>(buf);
	T *endIt = beginIt + (len/sizeof(T));
	auto offset = self.silenceLevel();

	for (auto sample = beginIt; sample != endIt; ++sample) {
		float signedSample = *sample - offset;

		float delaySum = 0;

		for (size_t i = 0; i < self.numDelays; ++i)
			delaySum += self.delayBuffer[i][self.bufPos[i]]; // sum all delay lines

		signedSample =
			(signedSample + (delaySum*self.decay)) / // add attenuated echos
			((float)1.0 + (float)(self.numDelays)*self.decay); // weighted average

		*sample = signedSample + offset;

		for (size_t i = 0; i < self.numDelays; ++i) {
			self.delayBuffer[i][self.bufPos[i]] = signedSample; // save the echo for the next time round
			self.bufPos[i] = (self.bufPos[i] + 1) % self.delayBuffer[i].size(); // circular echo/delay buffer
		}
	}
};

ReverbImpl::ReverbImpl(DecoderImpl *impl, float delay, float decay, size_t numDelays) :
		DecoderImplDecorator(impl->getFileImpl(), impl),
		delay(delay),
		decay(decay),
		numDelays(numDelays),
		delayBuffer(numDelays),
		bufPos(numDelays, 0) {

	reverb = reverbFunc[getBitsPerSample()];

	float delayInterval = 1.0/(float)numDelays;
	for (auto &delayLine: delayBuffer) {
		delayLine.resize(
			delay * delayInterval *
			(float)(getSampleRate() *
			(getBitsPerSample() >> 3) *
			getNumChannels()), 0);
		delayInterval += 1.0/(float)numDelays;
	}
}

int ReverbImpl::getSamples(char* buf, int pos, int len) {
	auto ret = impl->getSamples(buf, pos, len);

	if (pos == 0) { // clear delay buffers when playing begins
		for (auto &bp: bufPos)
			bp = 0;
		for (auto &delayLine: delayBuffer)
			std::fill(delayLine.begin(), delayLine.end(), 0);
	}

	if (ret > 0)
		reverb(*this, buf, pos, len);

	return ret;
}

} /* namespace jukebox */

