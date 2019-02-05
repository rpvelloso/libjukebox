/*
 * SampleResolutionImpl.h
 *
 *  Created on: 4 de fev de 2019
 *      Author: Benutzer
 */

#ifndef JUKEBOX_DECODERS_DECORATORS_SAMPLERESOLUTIONIMPL_H_
#define JUKEBOX_DECODERS_DECORATORS_SAMPLERESOLUTIONIMPL_H_

#include <memory>
#include <functional>
#include <unordered_map>
#include "../DecoderImpl.h"

namespace jukebox {

class SampleResolutionImpl : public DecoderImpl {
public:
	SampleResolutionImpl(DecoderImpl *impl, int resolution);
	virtual ~SampleResolutionImpl() = default;
	int getSamples(char *buf, int pos, int len) override;
	int getBlockSize() const override;
	short getBitsPerSample() const override;
	int getDataSize() const override;
	virtual int silenceLevel() const;
private:
	std::unique_ptr<DecoderImpl> impl;
	int resolution, nativeResolution;
	std::function<int(void *, int, int)> changeResolution;
	static std::unordered_map<int, decltype(changeResolution)> changeResolutionFunc;

	template<class T, class U> // converts from T to U
	static int _changeResolution(void *buf, int len, int offset);
};

} /* namespace jukebox */

#endif /* JUKEBOX_DECODERS_DECORATORS_SAMPLERESOLUTIONIMPL_H_ */
