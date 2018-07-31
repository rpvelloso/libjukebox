/*
 * FadeOnStopSound.cpp
 *
 *  Created on: 27 de jul de 2018
 *      Author: rvelloso
 */

#include <iostream>
#include <algorithm>
#include "FadeOnStopSoundImpl.h"

namespace jukebox {

class FadeOutOnStop : public SoundTransformation {
public:
	FadeOutOnStop(SoundFile &sf, int fadeOutSecs, int fadeOutStartPos) :
		SoundTransformation(sf),
		fadeOutSecs(fadeOutSecs),
		fadeOutStartPos(fadeOutStartPos) {

		int stopPos =
			fadeOutStartPos + (
			soundFile.getSampleRate()*
			soundFile.getNumChannels()*
			(soundFile.getBitsPerSample() == 8?1:2)*
			fadeOutSecs);

		if (stopPos < soundFile.getDataSize()) {
			fade = true;
			soundFile.truncAt(stopPos);
		}
	};

	void operator()(void *buf, int pos, int len) override {
		if (pos >= fadeOutStartPos) {
			if (soundFile.getBitsPerSample() == 8)
				fadeOut((uint8_t *)buf, pos, len);
			else
				fadeOut((int16_t *)buf, pos, len);
		}
	};
private:
	int fadeOutSecs, fadeOutStartPos;
	bool fade = false;
	template<typename T>
	void fadeOut(T *buf, int pos, int len) {
		if (!fade)
			return;

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
	};
};

FadeOnStopSoundImpl::FadeOnStopSoundImpl(SoundImpl *impl, int fadeOutSecs) :
		SoundImpl(impl->getSoundFile()),
		impl(impl),
		fadeOutSecs(fadeOutSecs) {
}

void FadeOnStopSoundImpl::play() {
	impl->play();
}

void FadeOnStopSoundImpl::stop() {
	if (impl->getPosition() > 0 && fadeOutSecs > 0)
		impl->setTransformation(new FadeOutOnStop(impl->getSoundFile(), fadeOutSecs, impl->getPosition()));
}

int FadeOnStopSoundImpl::getVolume() const {
	return impl->getVolume();
}

void FadeOnStopSoundImpl::setVolume(int vol) {
	impl->setVolume(vol);
}

void FadeOnStopSoundImpl::loop(bool loop) {
	impl->loop(loop);
}

} /* namespace jukebox */
