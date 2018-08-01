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

void effect(std::vector<float> &seq) {
	for (auto &v:seq)
		v = sqrt(v)*(v>0?1:-1);
}

BufferedSoundFileImpl::BufferedSoundFileImpl(SoundFileImpl* impl) :
	impl(impl),
	data(new char[impl->getDataSize()]) {

	dataSize = impl->getDataSize();
	impl->read(data.get(), 0, getDataSize());

	std::vector<float> fData;

	if (impl->getBitsPerSample() == 16) {
		fData = toFloat<int16_t>();
	} else {
		fData = toFloat<uint8_t>();
		dataSize *= 2; // toFixed will convert audio to signed 16 bits
		data.reset(new char[dataSize]);
	}

	//effect(fData);
	toFixed(fData);
}

short BufferedSoundFileImpl::getNumChannels() const {
	return impl->getNumChannels();
}

int BufferedSoundFileImpl::getSampleRate() const {
	return impl->getSampleRate();
}

short BufferedSoundFileImpl::getBitsPerSample() const {
	return 16;//impl->getBitsPerSample();//16;
}

int BufferedSoundFileImpl::getDataSize() const {
	return dataSize;
}

const std::string& BufferedSoundFileImpl::getFilename() const {
	return impl->getFilename();
}

int BufferedSoundFileImpl::read(char* buf, int pos, int len) {
	int n = getDataSize();
	int ret = ((pos + len) > n? n - pos : len);
	if (ret > 0)
		memcpy(buf, data.get() + pos, ret);
	return std::max(ret, 0);
}

template<typename T>
std::vector<float> BufferedSoundFileImpl::toFloat() {
	T *beginIt = reinterpret_cast<T *>(data.get());
	T *endIt = beginIt + (getDataSize()/sizeof(T));
	float maxPeak = 0;
	std::vector<float> result;

	result.reserve(std::distance(beginIt, endIt));

	std::for_each(beginIt, endIt,
		[&maxPeak](T &sample){
			if (std::abs(sample) > maxPeak)
				maxPeak = std::abs(sample);
	});

	int offset = 0;

	if (sizeof(T) == 1)
		offset = 128;

	maxPeak -= offset;
	std::for_each(beginIt, endIt,
		[maxPeak, offset, &result](T &sample){
			result.emplace_back(static_cast<float>(sample - offset) / maxPeak);
	});

	return result;
}

void BufferedSoundFileImpl::toFixed(std::vector<float> &inp) {
	int16_t *beginIt = reinterpret_cast<int16_t *>(data.get());
	int16_t *endIt = beginIt + (getDataSize()/sizeof(int16_t));
	auto inpIt = inp.begin();
	float maxFloat = 0;
	float maxFixed = std::numeric_limits<int16_t>::max();

	for (auto v: inp) {
		if (std::abs(v) > maxFloat)
			maxFloat = std::abs(v);
	}

	std::for_each(beginIt, endIt,
		[&inpIt, maxFixed, maxFloat](int16_t &sample){
			sample = static_cast<int16_t>((maxFixed * (*inpIt))/maxFloat);
			++inpIt;
	});
}

std::unique_ptr<Decoder> BufferedSoundFileImpl::makeDecoder() {
	return impl->makeDecoder();
}

} /* namespace jukebox */
