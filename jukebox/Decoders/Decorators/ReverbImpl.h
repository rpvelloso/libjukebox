/*
    Copyright 2017 Roberto Panerai Velloso.
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

#ifndef JUKEBOX_SOUND_REVERBIMPL_H_
#define JUKEBOX_SOUND_REVERBIMPL_H_

#include <memory>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "../DecoderImpl.h"

namespace jukebox {

class ReverbImpl: public DecoderImpl {
public:
	ReverbImpl(DecoderImpl *impl, float delay, float decay, size_t numDelays);
	virtual ~ReverbImpl() = default;
	int getSamples(char *buf, int pos, int len) override;
private:
	std::unique_ptr<DecoderImpl> impl;
	float delay, decay;
	size_t numDelays;

	std::vector<std::vector<float> > delayBuffer;
	std::vector<size_t> bufPos;

	std::function<void(ReverbImpl &, void *, int, int)> reverb;
	static std::unordered_map<short, decltype(reverb)> reverbFunc;

	template<typename T>
	static void _reverb(ReverbImpl &self, void *buf, int pos, int len);
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_REVERBIMPL_H_ */
