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
#include <iostream>
#include "SoundTransformation.h"
#include "Sound.h"

namespace {

int normalize(int vol) { return std::max(0,std::min(vol,100)); }

}

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

	void operator()(uint8_t *buf, int pos, int len) override {
		fadeOut(buf, pos, len);
	};
	void operator()(int16_t *buf, int pos, int len) override {
		fadeOut(buf, pos, len);
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

Sound::Sound(SoundImpl *impl) : impl(impl) {
}

void Sound::play() {
	impl->stop();
	impl->play();
}

void Sound::stop() {
	std::cout << "installing callback: " << getPosition() << std::endl;
	impl->setTransformation(new FadeOutOnStop(impl->getSoundFile(), 10, getPosition()));
	//impl->stop();
}

int Sound::getVolume() {
	auto vol = impl->getVolume();
	return normalize(vol);
}

void Sound::setVolume(int vol) {
	impl->setVolume(normalize(vol));
}

void Sound::loop(bool l) {
	impl->loop(l);
}

int Sound::getPosition() const {
	return impl->getPosition();
}

} /* namespace jukebox */
