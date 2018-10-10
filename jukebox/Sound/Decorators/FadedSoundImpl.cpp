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

#include "FadedSoundImpl.h"
#include <algorithm>
#include "jukebox/Sound/SoundTransformation.h"

namespace jukebox {

class Fade : public SoundTransformation {
public:
	Fade(SoundFile &sf, int fadeInSecs, int fadeOutSecs) :
		SoundTransformation(sf),
		fadeInSecs(fadeInSecs),
		fadeOutSecs(fadeOutSecs),
		fadeInEndPos(0),
		fadeOutStartPos(sf.getDataSize()) {

		if (fadeInSecs > 0)
			fadeInEndPos = std::min(
					sf.getSampleRate()*
					sf.getNumChannels()*
					(sf.getBitsPerSample() == 8?1:2)*
					fadeInSecs, sf.getDataSize());

		if (fadeOutSecs > 0)
			fadeOutStartPos -= std::min(
					sf.getSampleRate()*
					sf.getNumChannels()*
					(sf.getBitsPerSample() == 8?1:2)*
					fadeOutSecs, sf.getDataSize());
	};

	void operator()(void *buf, int pos, int len) {
		auto dataSize = soundFile.getDataSize();

		if (pos >= dataSize)
			return;

		if (pos + len > dataSize)
			len = (dataSize - pos);

		if (pos < fadeInEndPos) {
			if (soundFile.getBitsPerSample() == 16)
				fadeIn((int16_t *)buf, pos, len);
			else
				fadeIn((uint8_t *)buf, pos, len);
		}

		if (pos >= fadeOutStartPos) {
			if (soundFile.getBitsPerSample() == 16)
				fadeOut((int16_t *)buf, pos, len);
			else
				fadeOut((uint8_t *)buf, pos, len);
		}
	};
private:
	int fadeInSecs, fadeOutSecs;
	int fadeInEndPos, fadeOutStartPos;

	template<typename T>
	void fadeIn(T* buf, int pos, int len) {
		T *beginIt = reinterpret_cast<T *>(buf);
		T *endIt = beginIt + (len/sizeof(T));

		int offset = 0;
		if (sizeof(T) == 1)
			offset = 128;

		std::for_each(beginIt, endIt, [this, offset, &pos](T &sample){
			sample = (T)((((float)(sample - offset) * (float)pos)/(float)fadeInEndPos) + offset);
			++pos;
		});
	}

	template<typename T>
	void fadeOut(T* buf, int pos, int len) {
		T *beginIt = reinterpret_cast<T *>(buf);
		T *endIt = beginIt + (len/sizeof(T));

		auto n = soundFile.getDataSize();
		auto fadeLen = n - fadeOutStartPos;

		int offset = 0;
		if (sizeof(T) == 1)
			offset = 128;

		std::for_each(beginIt, endIt, [this, n, fadeLen, offset, &pos](T &sample){
			sample = (T)((((float)(sample - offset) * (float)(n - pos))/(float)(fadeLen)) + offset);
			++pos;
		});
	}

};

FadedSoundImpl::FadedSoundImpl(
		SoundImpl *impl,
		int fadeInSecs,
		int fadeOutSecs) :
	SoundImpl(impl->getSoundFile()),
	impl(impl) {

	impl->setTransformationCallback(Fade(soundFile, fadeInSecs, fadeOutSecs));
}

void FadedSoundImpl::play() {
	impl->play();
}

void FadedSoundImpl::stop() {
	impl->stop();
}

int FadedSoundImpl::getVolume() const {
	return impl->getVolume();
}

void FadedSoundImpl::setVolume(int vol) {
	impl->setVolume(vol);
}

void FadedSoundImpl::loop(bool loop) {
	impl->loop(loop);
}

void FadedSoundImpl::setOnStopCallback(std::function<void(void)> os) {
	impl->setOnStopCallback(os);
}

} /* namespace jukebox */
