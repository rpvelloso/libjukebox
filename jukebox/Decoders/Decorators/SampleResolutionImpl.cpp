/*
 * SampleResolutionImpl.cpp
 *
 *  Created on: 4 de fev de 2019
 *      Author: Benutzer
 */

#include <limits>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "SampleResolutionImpl.h"

namespace jukebox {

std::unordered_map<int, decltype(SampleResolutionImpl::changeResolution)>
	SampleResolutionImpl::changeResolutionFunc = {
			{0      , [](void *buf, void *out, int len, int offset){
				std::copy((char *)buf, (char *)buf+len, (char *)out);
				return len;
			}},
			{32 - 16, SampleResolutionImpl::_changeResolution<int32_t, int16_t>},
			{16 - 32, SampleResolutionImpl::_changeResolution<int16_t, int32_t>},
			{32 -  8, SampleResolutionImpl::_changeResolution<int32_t, uint8_t>},
			{8  - 32, SampleResolutionImpl::_changeResolution<uint8_t, int32_t>},
			{16 -  8, SampleResolutionImpl::_changeResolution<int16_t, uint8_t>},
			{8  - 16, SampleResolutionImpl::_changeResolution<uint8_t, int16_t>}
	};

SampleResolutionImpl::SampleResolutionImpl(DecoderImpl* impl, int resolution) :
	DecoderImplDecorator(impl->getFileImpl(), impl),
	resolution(resolution),
	nativeResolution(impl->getBitsPerSample()) {

	if (resolution != 8 && resolution != 16 && resolution != 32)
		throw std::runtime_error("invalid resolution: " + std::to_string(resolution));

	changeResolution = changeResolutionFunc[nativeResolution - resolution];
}

int SampleResolutionImpl::getSamples(char* buf, int pos, int len) {
	std::unique_ptr<char []> resBuf(new char[len*nativeResolution/resolution]);

	auto siz = impl->getSamples(
		resBuf.get(),
		pos*nativeResolution/resolution,
		len*nativeResolution/resolution);

	auto chsiz = changeResolution(resBuf.get(), buf, siz, impl->silenceLevel());
	return chsiz;
}

template<class T, class U>
int SampleResolutionImpl::_changeResolution(void* buf, void *out, int len, int offset) {
	if (len <= 0)
		return len;

	T *inp = reinterpret_cast<T *>(buf);
	U *outp = reinterpret_cast<U *>(out);

	auto beginIt = inp;
	auto endIt = inp + (len / sizeof(T));
	for (auto sample = beginIt; sample != endIt; ++sample, ++outp) {
		double signedSample = ((double)(*sample - offset) / (double)std::numeric_limits<T>::max());
		double convertedSample = signedSample * (double)std::numeric_limits<U>::max();
		*outp = convertedSample + offset;
	}

	return (len*sizeof(U))/sizeof(T);
}

int SampleResolutionImpl::getBlockSize() const {
	return (impl->getBlockSize()*resolution)/nativeResolution;
}

short SampleResolutionImpl::getBitsPerSample() const {
	return resolution;
}

int SampleResolutionImpl::getDataSize() const {
	return (impl->getDataSize()*resolution)/nativeResolution;
}

int SampleResolutionImpl::silenceLevel() const {
	return resolution == 8?128:0;
}

} /* namespace jukebox */
