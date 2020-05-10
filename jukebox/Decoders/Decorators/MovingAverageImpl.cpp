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

#include <iostream>
#include <algorithm>
#include "MovingAverageImpl.h"

namespace jukebox {

std::vector<std::unordered_map<int, decltype(MovingAverageImpl::movingAverage)>> MovingAverageImpl::movingAverageFunc = {
	{
		{8,  &MovingAverageImpl::_movingAverageMono<uint8_t>},
		{16, &MovingAverageImpl::_movingAverageMono<int16_t>},
		{32, &MovingAverageImpl::_movingAverageMono<int32_t>}
	},
	{
		{8,  &MovingAverageImpl::_movingAverageStereo<uint8_t>},
		{16, &MovingAverageImpl::_movingAverageStereo<int16_t>},
		{32, &MovingAverageImpl::_movingAverageStereo<int32_t>}
	}
};

MovingAverageImpl::MovingAverageImpl(DecoderImpl *impl, float windowLength) :
			DecoderImplDecorator(impl->getFileImpl(), impl),
			n_samples(std::max((int)(windowLength * impl->getSampleRate()), 1)),
			movingAverage(movingAverageFunc[impl->getNumChannels() - 1][impl->getBitsPerSample()]) {
}

template<typename T>
double rollingAverage(double &prevAvg, T currentValue, int n) {
	//return - (prevAvg/(double)n) + (currentValue / n);

	//return (prevAvg*(double)(n-1) + (double)currentValue) / (double)n;
	//https://stackoverflow.com/questions/12636613/how-to-calculate-moving-average-without-keeping-the-count-and-data-total

	return prevAvg + ((double)currentValue - prevAvg) / (double)n;
	//https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online
}

template<typename T>
void MovingAverageImpl::_movingAverageMono(MovingAverageImpl &self, void *buf, int len) {
	auto beginIt = reinterpret_cast<T *>(buf);
	auto endIt = beginIt + (len / sizeof(T));

	std::for_each(beginIt, endIt, [&buf, &self](T &value) {
		self.avg[0] = rollingAverage(self.avg[0], value, self.n_samples);
		value = self.avg[0];
	});
}

template<typename T>
void MovingAverageImpl::_movingAverageStereo(MovingAverageImpl &self, void *buf, int len) {
	auto beginIt = reinterpret_cast<T *>(buf);
	auto endIt = beginIt + (len / sizeof(T));

	for (auto it = beginIt; it != endIt; it += 2) {
		auto left = it;
		auto right = left + 1;
		self.avg[0] = rollingAverage(self.avg[0], *left, self.n_samples);
		self.avg[1] = rollingAverage(self.avg[1], *right, self.n_samples);
		*left = self.avg[0];
		*right = self.avg[1];
	}
}

int MovingAverageImpl::getSamples(char *buf, int pos, int len) {
	len = impl->getSamples(buf, pos, len);
	movingAverage(*this, static_cast<void *>(buf), len);
	return len;
}

} /* namespace jukebox */
