/*
 * DistortionImpl.cpp
 *
 *  Created on: 5 de fev de 2019
 *      Author: rvelloso
 */

#include <limits>
#include <cmath>
#include "DistortionImpl.h"

namespace jukebox {

std::unordered_map<int, decltype(DistortionImpl::distortion)> DistortionImpl::distortionFunc = {
	{ 8, DistortionImpl::_distortion<uint8_t>},
	{16, DistortionImpl::_distortion<int16_t>},
	{32, DistortionImpl::_distortion<int32_t>}
};

DistortionImpl::DistortionImpl(DecoderImpl* impl, float gain) :
	DecoderImpl(impl->getFileImpl()),
	impl(impl),
	gain(gain) {

	distortion = distortionFunc[impl->getBitsPerSample()];
}

int DistortionImpl::getSamples(char* buf, int pos, int len) {
	auto ret = impl->getSamples(buf, pos, len);
	distortion(buf, len, impl->silenceLevel(), gain);
	return ret;
}

template<class T>
inline void DistortionImpl::_distortion(void* buf, int len, int offset, float gain) {
	auto beginIt = reinterpret_cast<T *>(buf);
	auto endIt = beginIt + len/(sizeof(T));

	for (auto sample = beginIt; sample != endIt; ++sample) {
		float signedSample = (float)(*sample - offset) / (float)std::numeric_limits<T>::max();

		signedSample = std::tanh(gain*signedSample)/std::tanh(gain);

		*sample = (signedSample * (float)std::numeric_limits<T>::max()) + offset;
	}
}

} /* namespace jukebox */
