/*
 * SlidingWindowImpl.h
 *
 *  Created on: 3 de set de 2019
 *      Author: rvelloso
 */

#ifndef JUKEBOX_DECODERS_DECORATORS_MOVINGAVERAGEIMPL_H_
#define JUKEBOX_DECODERS_DECORATORS_MOVINGAVERAGEIMPL_H_

#include <vector>
#include <functional>
#include <unordered_map>
#include "DecoderImplDecorator.h"

namespace jukebox {

class MovingAverageImpl: public DecoderImplDecorator {
public:
	MovingAverageImpl(DecoderImpl *impl, float windowLength); // window length in seconds (or fractions)
	virtual ~MovingAverageImpl() = default;
	int getSamples(char* buf, int pos, int len) override;
private:
	int n_samples;
	double avg[2] = {0, 0}; // left & right channels

	std::function<void(MovingAverageImpl &, void *, int)> movingAverage;
	static std::vector<std::unordered_map<int, decltype(movingAverage)>> movingAverageFunc;

	template<typename T>
	static void _movingAverageMono(MovingAverageImpl &, void *buf, int len);
	template<typename T>
	static void _movingAverageStereo(MovingAverageImpl &, void *buf, int len);
};

} /* namespace jukebox */

#endif /* JUKEBOX_DECODERS_DECORATORS_MOVINGAVERAGEIMPL_H_ */
