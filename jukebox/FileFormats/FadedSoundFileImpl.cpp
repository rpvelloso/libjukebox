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
				fadeOutStartPos(impl->getDataSize()) {
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
	return impl->getDataSize();
}

const std::string& FadedSoundFileImpl::getFilename() const {
	return impl->getFilename();
}

int FadedSoundFileImpl::read(char* buf, int pos, int len) {
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

	std::for_each(beginIt, endIt, [this, &pos](T &sample){
		sample = (T)(((float)sample * (float)pos)/(float)fadeInEndPos);
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

	std::for_each(beginIt, endIt, [this, n, fadeLen, &pos](T &sample){
		sample = (T)(((float)sample * (float)(n - pos))/(float)(fadeLen));
		++pos;
	});
}

} /* namespace jukebox */
