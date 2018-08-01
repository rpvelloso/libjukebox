/*
 * FadedSoundImpl.cpp
 *
 *  Created on: 1 de ago de 2018
 *      Author: rvelloso
 */

#include <algorithm>
#include "FadedSoundImpl.h"

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

	impl->setTransformation(Fade(soundFile, fadeInSecs, fadeOutSecs));
}

void FadedSoundImpl::play() {
	impl->play();
}

void FadedSoundImpl::stop() {
	impl->stop();
}

int FadedSoundImpl::getVolume() {
	return impl->getVolume();
}

void FadedSoundImpl::setVolume(int vol) {
	impl->setVolume(vol);
}

void FadedSoundImpl::loop(bool loop) {
	impl->loop(loop);
}

} /* namespace jukebox */

