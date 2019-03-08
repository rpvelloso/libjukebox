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

#ifndef JUKEBOX_DECODERS_DECORATORS_SAMPLERESOLUTIONIMPL_H_
#define JUKEBOX_DECODERS_DECORATORS_SAMPLERESOLUTIONIMPL_H_

#include <memory>
#include <functional>
#include <unordered_map>
#include "../DecoderImpl.h"
#include "DecoderImplDecorator.h"

namespace jukebox {

class SampleResolutionImpl : public DecoderImplDecorator {
public:
	SampleResolutionImpl(DecoderImpl *impl, int resolution);
	virtual ~SampleResolutionImpl() = default;
	int getSamples(char *buf, int pos, int len) override;
	int getBlockSize() const override;
	short getBitsPerSample() const override;
	int getDataSize() const override;
	virtual int silenceLevel() const;
private:
	int resolution, nativeResolution;
	std::function<int(void *, int, int)> changeResolution;
	static std::unordered_map<int, decltype(changeResolution)> changeResolutionFunc;

	template<class T, class U> // converts from T to U
	static int _changeResolution(void *buf, int len, int offset);
};

} /* namespace jukebox */

#endif /* JUKEBOX_DECODERS_DECORATORS_SAMPLERESOLUTIONIMPL_H_ */
