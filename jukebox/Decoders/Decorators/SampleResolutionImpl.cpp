/*
    Copyright 2019 Roberto Panerai Velloso.
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

#include <limits>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "SampleResolutionImpl.h"

namespace jukebox {

std::unordered_map<int, decltype(SampleResolutionImpl::changeResolution)>
	SampleResolutionImpl::changeResolutionFunc = {
			{0      , [](void *buf, void *out, int len, int source_offset, int dest_offset){
				std::copy((char *)buf, (char *)buf+len, (char *)out);
				return len;
			}},
			{32 - 16, SampleResolutionImpl::_changeResolution<int32_t, int16_t>}, // 16
			{16 - 32, SampleResolutionImpl::_changeResolution<int16_t, int32_t>}, // -16
			{32 -  8, SampleResolutionImpl::_changeResolution<int32_t, uint8_t>}, // 24
			{8  - 32, SampleResolutionImpl::_changeResolution<uint8_t, int32_t>}, // -24
			{16 -  8, SampleResolutionImpl::_changeResolution<int16_t, uint8_t>}, // 8
			{8  - 16, SampleResolutionImpl::_changeResolution<uint8_t, int16_t>} // -8
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

	auto chsiz = changeResolution(resBuf.get(), buf, siz, impl->silenceLevel(), silenceLevel());
	return chsiz;
}

template<class T, class U>
int SampleResolutionImpl::_changeResolution(
		void* buf,
		void *out,
		int len,
		int source_offset,
		int dest_offset) {
	if (len <= 0)
		return len;

	T *inp = reinterpret_cast<T *>(buf);
	U *outp = reinterpret_cast<U *>(out);

	auto beginIt = inp;
	auto endIt = inp + (len / sizeof(T));
	for (auto sample = beginIt; sample != endIt; ++sample, ++outp) {
		double signedSample = ((double)(*sample - source_offset) / (double)std::numeric_limits<T>::max());
		double convertedSample = signedSample * (double)std::numeric_limits<U>::max();
		*outp = convertedSample + dest_offset;
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
