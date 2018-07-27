/*
 * FadedSoundFileImpl.cpp
 *
 *  Created on: 26 de jul de 2018
 *      Author: rvelloso
 */

#include <iostream>
#include <algorithm>
#include "FadedSoundFileImpl.h"

namespace jukebox {

FadedSoundFileImpl::FadedSoundFileImpl(
		SoundFileImpl *impl, int fadeInSecs, int fadeOutSecs) :
				impl(impl),
				fadeInSecs(fadeInSecs),
				fadeOutSecs(fadeOutSecs),
				fadeInEndPos(0),
				fadeOutStartPos(impl->getDataSize()),
				dataSize(impl->getDataSize()) {

	if (fadeInSecs > 0)
		fadeInEndPos = std::min(
				impl->getSampleRate()*
				impl->getNumChannels()*
				(impl->getBitsPerSample() == 8?1:2)*
				fadeInSecs, impl->getDataSize());

	if (fadeOutSecs > 0)
		fadeOutStartPos -= std::min(
				impl->getSampleRate()*
				impl->getNumChannels()*
				(impl->getBitsPerSample() == 8?1:2)*
				fadeOutSecs, impl->getDataSize());
}

short FadedSoundFileImpl::getNumChannels() const {
	return impl->getNumChannels();
}

int FadedSoundFileImpl::getSampleRate() const {
	return impl->getSampleRate();
}

short FadedSoundFileImpl::getBitsPerSample() const {
	return impl->getBitsPerSample();
}

int FadedSoundFileImpl::getDataSize() const {
	return dataSize;
}

const std::string& FadedSoundFileImpl::getFilename() const {
	return impl->getFilename();
}

int FadedSoundFileImpl::read(char* buf, int pos, int len) {
	if (pos >= dataSize)
		return 0;
	if (pos + len > dataSize)
		len = (dataSize - pos);

	auto ret = impl->read(buf, pos, len);
	if (pos < fadeInEndPos) {
		if (impl->getBitsPerSample() == 16)
			fadeIn<int16_t>(buf, pos, ret);
		else
			fadeIn<uint8_t>(buf, pos, ret);
	}

	if (pos >= fadeOutStartPos) {
		if (impl->getBitsPerSample() == 16)
			fadeOut<int16_t>(buf, pos, ret);
		else
			fadeOut<uint8_t>(buf, pos, ret);
	}
	return ret;
}

template<typename T>
void FadedSoundFileImpl::fadeIn(char* buf, int pos, int len) {
	T *beginIt = reinterpret_cast<T *>(buf);
	T *endIt = beginIt + (len/sizeof(T));

	/*std::cout <<
			(float)pos/(float)fadeInEndPos << " - " <<
			(float)(pos + len)/(float)fadeInEndPos <<
			std::endl;*/

	int offset = 0;
	if (sizeof(T) == 1)
		offset = 128;

	std::for_each(beginIt, endIt, [this, offset, &pos](T &sample){
		sample = (T)((((float)(sample - offset) * (float)pos)/(float)fadeInEndPos) + offset);
		++pos;
	});
}

template<typename T>
void FadedSoundFileImpl::fadeOut(char* buf, int pos, int len) {
	T *beginIt = reinterpret_cast<T *>(buf);
	T *endIt = beginIt + (len/sizeof(T));

	auto n = impl->getDataSize();
	auto fadeLen = n - fadeOutStartPos;

	/*std::cout <<
			(float)(n - pos)/(float)fadeLen << " - " <<
			(float)(n - pos + len)/(float)fadeLen <<
			std::endl;*/

	int offset = 0;
	if (sizeof(T) == 1)
		offset = 128;

	std::for_each(beginIt, endIt, [this, n, fadeLen, offset, &pos](T &sample){
		sample = (T)((((float)(sample - offset) * (float)(n - pos))/(float)(fadeLen)) + offset);
		++pos;
	});
}

void FadedSoundFileImpl::setFadeOutStartPos(int fadeOutStartPos) {
	if (fadeOutStartPos > fadeInEndPos && fadeOutStartPos < this->fadeOutStartPos) {
		dataSize = fadeOutStartPos + (dataSize - this->fadeOutStartPos);
		this->fadeOutStartPos = fadeOutStartPos;
	}
}

} /* namespace jukebox */
