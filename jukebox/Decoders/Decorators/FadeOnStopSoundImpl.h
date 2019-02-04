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

#ifndef JUKEBOX_SOUND_FADEONSTOPSOUNDIMPL_H_
#define JUKEBOX_SOUND_FADEONSTOPSOUNDIMPL_H_

#include <functional>
#include <memory>
#include <algorithm>
#include <unordered_map>

#include "../DecoderImpl.h"

namespace jukebox {

class FadeOnStopSoundImpl: public DecoderImpl {
public:
	FadeOnStopSoundImpl(DecoderImpl *, int, int);
	virtual ~FadeOnStopSoundImpl() = default;
	int getSamples(char *buf, int pos, int len) override;
private:
	std::unique_ptr<DecoderImpl> impl;
	int fadeOutSecs, fadeOutStartPos;
	bool fade = false;
	std::function<void(FadeOnStopSoundImpl&, void *, int, int)> fadeOut;
	static std::unordered_map<short, decltype(fadeOut)> fadeOutFunc;

	template<typename T>
	static void _fadeOut(FadeOnStopSoundImpl& self, void *buf, int pos, int len) {
		if (!self.fade)
			return;

		T *beginIt = reinterpret_cast<T *>(buf);
		T *endIt = beginIt + (len/sizeof(T));

		auto n = self.getDataSize();
		auto fadeLen = n - self.fadeOutStartPos;

		int offset = self.silenceLevel();

		std::for_each(beginIt, endIt, [n, fadeLen, offset, &pos](T &sample){
			sample = (T)((((float)(sample - offset) * (float)(n - pos))/(float)(fadeLen)) + offset);
			++pos;
		});
	};
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_FADEONSTOPSOUNDIMPL_H_ */
