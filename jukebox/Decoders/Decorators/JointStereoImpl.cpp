/*
 * JointStereoImpl.cpp
 *
 *  Created on: 2 de set de 2019
 *      Author: rvelloso
 */

#include "JointStereoImpl.h"

namespace jukebox {

std::unordered_map<int, decltype(JointStereoImpl::mixChannels)>
JointStereoImpl::mixChannelsFunc = {
			{8, JointStereoImpl::_mixChannels<uint8_t>},
			{16, JointStereoImpl::_mixChannels<int16_t>},
			{32, JointStereoImpl::_mixChannels<int32_t>}
	};

JointStereoImpl::JointStereoImpl(DecoderImpl *impl) :
		DecoderImplDecorator(impl->getFileImpl(), impl) {

	mixChannels = mixChannelsFunc[impl->getBitsPerSample()];
}

template <typename T>
void JointStereoImpl::_mixChannels(void *input, void *output, int len) {
	T *inp = reinterpret_cast<T *>(input);
	T *outp = reinterpret_cast<T *>(output);
	auto beginIt = inp;
	auto endIt = beginIt + (len / sizeof(T));
	for (auto it = beginIt; it != endIt; it += 2, ++outp) {
		auto left = it;
		auto right = it+1;
		*outp = (*left + *right) / 2;
	}
}

int JointStereoImpl::getSamples(char* buf, int pos, int len) {
	std::unique_ptr<char []> resBuf(new char[len*2]);

	auto result = impl->getSamples(resBuf.get(), pos*2, len*2);
	mixChannels(resBuf.get(), buf, result);
	return result/2;
}

short JointStereoImpl::getNumChannels() const {
	return 1;
}

} /* namespace jukebox */
