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
