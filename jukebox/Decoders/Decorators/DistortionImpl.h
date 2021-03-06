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

#ifndef JUKEBOX_DECODERS_DECORATORS_DISTORTIONIMPL_H_
#define JUKEBOX_DECODERS_DECORATORS_DISTORTIONIMPL_H_

#include <memory>
#include <functional>
#include <unordered_map>
#include "../DecoderImpl.h"
#include "DecoderImplDecorator.h"

namespace jukebox {

class DistortionImpl: public DecoderImplDecorator {
public:
	DistortionImpl(DecoderImpl *impl, float gain);
	virtual ~DistortionImpl() = default;
	int getSamples(char* buf, int pos, int len) override;
private:
	float gain;
	std::function<void(void *, int, int, float)> distortion;
	static std::unordered_map<int, decltype(distortion)> distortionFunc;

	template<class T>
	static void _distortion(void *buf, int len, int offset, float gain);
};

} /* namespace jukebox */

#endif /* JUKEBOX_DECODERS_DECORATORS_DISTORTIONIMPL_H_ */
