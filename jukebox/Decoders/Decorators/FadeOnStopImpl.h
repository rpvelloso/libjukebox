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

#ifndef JUKEBOX_SOUND_FADEONSTOPIMPL_H_
#define JUKEBOX_SOUND_FADEONSTOPIMPL_H_

#include <functional>
#include <memory>
#include <algorithm>
#include <unordered_map>

#include "../DecoderImpl.h"

namespace jukebox {

class FadeOnStopImpl: public DecoderImpl {
public:
	FadeOnStopImpl(DecoderImpl *, int, int);
	virtual ~FadeOnStopImpl() = default;
	int getSamples(char *buf, int pos, int len) override;
private:
	std::unique_ptr<DecoderImpl> impl;
	int fadeOutSecs, fadeOutStartPos;
	bool fade = false;
	std::function<void(FadeOnStopImpl&, void *, int, int)> fadeOut;
	static std::unordered_map<short, decltype(fadeOut)> fadeOutFunc;

	template<typename T>
	static void _fadeOut(FadeOnStopImpl& self, void *buf, int pos, int len);
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_FADEONSTOPIMPL_H_ */
