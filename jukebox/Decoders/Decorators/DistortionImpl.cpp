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
