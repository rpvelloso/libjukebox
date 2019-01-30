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
#include <unordered_map>
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
					(sf.getBitsPerSample() >> 3)*
					fadeInSecs, sf.getDataSize());

		if (fadeOutSecs > 0)
			fadeOutStartPos -= std::min(
					sf.getSampleRate()*
					sf.getNumChannels()*
					(sf.getBitsPerSample() >> 3)*
					fadeOutSecs, sf.getDataSize());

		fadeIn = fadeInFunc[soundFile.getBitsPerSample()];
		fadeOut = fadeOutFunc[soundFile.getBitsPerSample()];
	};

	void operator()(void *buf, int pos, int len) {
		auto dataSize = soundFile.getDataSize();

		if (pos >= dataSize)
			return;

		if (pos + len > dataSize)
			len = (dataSize - pos);

		if (pos < fadeInEndPos)
			fadeIn(*this, buf, pos, len);

		if (pos >= fadeOutStartPos)
			fadeOut(*this, buf, pos, len);
	};
private:
	int fadeInSecs, fadeOutSecs;
	int fadeInEndPos, fadeOutStartPos;
	std::function<void(Fade&, void *, int, int)> fadeIn;
	std::function<void(Fade&, void *, int, int)> fadeOut;
	static std::unordered_map<short, decltype(fadeIn)> fadeInFunc;
	static std::unordered_map<short, decltype(fadeOut)> fadeOutFunc;

	template<typename T>
	static void _fadeIn(Fade &self, void* buf, int pos, int len) {
		T *beginIt = reinterpret_cast<T *>(buf);
		T *endIt = beginIt + (len/sizeof(T));

		int offset = self.soundFile.silenceLevel();

		std::for_each(beginIt, endIt, [&self, offset, &pos](T &sample){
			sample = (T)((((float)(sample - offset) * (float)pos)/(float)self.fadeInEndPos) + offset);
			++pos;
		});
	}

	template<typename T>
	static void _fadeOut(Fade &self, void* buf, int pos, int len) {
		T *beginIt = reinterpret_cast<T *>(buf);
		T *endIt = beginIt + (len/sizeof(T));

		auto n = self.soundFile.getDataSize();
		auto fadeLen = n - self.fadeOutStartPos;

		int offset = self.soundFile.silenceLevel();

		std::for_each(beginIt, endIt, [n, fadeLen, offset, &pos](T &sample){
			sample = (T)((((float)(sample - offset) * (float)(n - pos))/(float)(fadeLen)) + offset);
			++pos;
		});
	}

};

std::unordered_map<short, decltype(Fade::fadeIn)> Fade::fadeInFunc = {
		{ 8, &Fade::_fadeIn<uint8_t>},
		{16, &Fade::_fadeIn<int16_t>},
		{32, &Fade::_fadeIn<int32_t>},
};
std::unordered_map<short, decltype(Fade::fadeOut)> Fade::fadeOutFunc = {
		{ 8, &Fade::_fadeOut<uint8_t>},
		{16, &Fade::_fadeOut<int16_t>},
		{32, &Fade::_fadeOut<int32_t>},
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
