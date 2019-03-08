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

#include "FadeOnStopImpl.h"

#include "jukebox/FileFormats/SoundFileImpl.h"

namespace jukebox {

template<typename T>
void FadeOnStopImpl::_fadeOut(FadeOnStopImpl& self, void *buf, int pos, int len) {
	if (!self.fade)
		return;

	T *beginIt = reinterpret_cast<T *>(buf);
	T *endIt = beginIt + (len/sizeof(T));

	auto n = self.getDataSize();
	auto fadeLen = n - self.fadeOutStartPos;

	int offset = self.silenceLevel();

	std::for_each(beginIt, endIt, [n, fadeLen, offset, &pos](T &sample){
		sample = (T)((((float)(sample - offset) * (float)(n - pos))/(float)(fadeLen)) + offset);
		++pos;
	});
};

std::unordered_map<short, decltype(FadeOnStopImpl::fadeOut)> FadeOnStopImpl::fadeOutFunc = {
		{8 , &FadeOnStopImpl::_fadeOut<uint8_t>},
		{16, &FadeOnStopImpl::_fadeOut<int16_t>},
		{32, &FadeOnStopImpl::_fadeOut<int32_t>}
};

FadeOnStopImpl::FadeOnStopImpl(DecoderImpl *impl, int fadeOutSecs, int fadeOutStartPos) :
		DecoderImplDecorator(impl->getFileImpl(), impl),
		fadeOutSecs(fadeOutSecs),
		fadeOutStartPos(fadeOutStartPos) {

	int stopPos =
		fadeOutStartPos + (
		getSampleRate()*
		getNumChannels()*
		(getBitsPerSample() >> 3)*
		fadeOutSecs);

	if (stopPos < getDataSize()) {
		fade = true;
		fileImpl.truncAt(stopPos);
	}

	fadeOut = fadeOutFunc[getBitsPerSample()];
}

/*void FadeOnStopSoundImpl::stop() {
	if (impl->getPosition() > 0 && fadeOutSecs > 0)
		impl->setTransformationCallback(FadeOutOnStop(impl->getSoundFile(), fadeOutSecs, impl->getPosition()));
}*/

int FadeOnStopImpl::getSamples(char* buf, int pos, int len) {
	auto ret = impl->getSamples(buf, pos, len);

	if (ret > 0) {
		if (pos >= fadeOutStartPos)
			fadeOut(*this, buf, pos, len);
	}

	return ret;
}

} /* namespace jukebox */

