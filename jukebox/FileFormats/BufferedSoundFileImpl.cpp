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

#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>
#include <cstdint>
#include <cstring>

#include "BufferedSoundFileImpl.h"

namespace jukebox {

BufferedSoundFileImpl::BufferedSoundFileImpl(SoundFileImpl* impl) :
	impl(impl),
	data(new char[this->impl->getDataSize()]) {

	this->impl->read(data.get(), 0, getDataSize());

	if (this->impl->getBitsPerSample() == 16)
		normalize<int16_t>();
	else
		normalize<uint8_t>();
}

short BufferedSoundFileImpl::getNumChannels() const {
	return impl->getNumChannels();
}

int BufferedSoundFileImpl::getSampleRate() const {
	return impl->getSampleRate();
}

short BufferedSoundFileImpl::getBitsPerSample() const {
	return impl->getBitsPerSample();
}

const char* BufferedSoundFileImpl::getData() const {
	return data.get();
}

int BufferedSoundFileImpl::getDataSize() const {
	return impl->getDataSize();
}

const std::string& BufferedSoundFileImpl::getFilename() const {
	return impl->getFilename();
}

int BufferedSoundFileImpl::read(char* buf, int pos, int len) {
	int n = getDataSize();
	int ret = ((pos + len) > n? n - pos : len);
	memcpy(buf, data.get() + pos, ret);
	std::cerr << n << " " << pos << " " << ret << std::endl;
	return std::max(ret, 0);
}

template<typename T>
void BufferedSoundFileImpl::normalize() {
	auto maxValue = std::numeric_limits<T>::max();
	auto maxPeak = std::numeric_limits<T>::min();
	auto beginIt = (T *)data.get();
	auto endIt = (T *)(data.get() + (impl->getDataSize()/sizeof(T)));

	std::for_each(beginIt, endIt,
		[&maxPeak](T &sample){
			if (sample > maxPeak)
				maxPeak = sample;
	});

	double ratio = static_cast<double>(maxValue)/static_cast<double>(maxPeak);
	std::for_each(
		beginIt,
		endIt,
		[ratio](T &sample){
			sample = static_cast<T>(static_cast<double>(sample)*ratio);
	});
}

} /* namespace jukebox */
