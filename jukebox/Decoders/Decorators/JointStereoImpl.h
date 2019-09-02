/*
 * JointStereoImpl.h
 *
 *  Created on: 2 de set de 2019
 *      Author: rvelloso
 */

#ifndef JUKEBOX_DECODERS_DECORATORS_JOINTSTEREOIMPL_H_
#define JUKEBOX_DECODERS_DECORATORS_JOINTSTEREOIMPL_H_

#include <unordered_map>
#include <functional>
#include "DecoderImplDecorator.h"

namespace jukebox {

class JointStereoImpl : public DecoderImplDecorator {
public:
	JointStereoImpl(DecoderImpl *impl);
	virtual ~JointStereoImpl() = default;
	int getSamples(char *buf, int pos, int len) override;
	short getNumChannels() const override;
private:
	std::function<void(void *, void *, int)> mixChannels;
	static std::unordered_map<int, decltype(mixChannels)> mixChannelsFunc;

	template <typename T>
	static void _mixChannels(void *input, void *output, int len);
};

} /* namespace jukebox */

#endif /* JUKEBOX_DECODERS_DECORATORS_JOINTSTEREOIMPL_H_ */
