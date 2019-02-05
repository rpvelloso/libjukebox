/*
 * DistortionImpl.h
 *
 *  Created on: 5 de fev de 2019
 *      Author: rvelloso
 */

#ifndef JUKEBOX_DECODERS_DECORATORS_DISTORTIONIMPL_H_
#define JUKEBOX_DECODERS_DECORATORS_DISTORTIONIMPL_H_

#include <memory>
#include <functional>
#include <unordered_map>
#include "../DecoderImpl.h"

namespace jukebox {

class DistortionImpl: public DecoderImpl {
public:
	DistortionImpl(DecoderImpl *impl, float gain);
	virtual ~DistortionImpl() = default;
	int getSamples(char* buf, int pos, int len) override;
private:
	std::unique_ptr<DecoderImpl> impl;
	float gain;
	std::function<void(void *, int, int, float)> distortion;
	static std::unordered_map<int, decltype(distortion)> distortionFunc;

	template<class T>
	static void _distortion(void *buf, int len, int offset, float gain);
};

} /* namespace jukebox */

#endif /* JUKEBOX_DECODERS_DECORATORS_DISTORTIONIMPL_H_ */
