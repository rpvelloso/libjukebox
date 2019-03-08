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

#ifndef JUKEBOX_SOUND_DECORATORS_FADEDSOUNDIMPL_H_
#define JUKEBOX_SOUND_DECORATORS_FADEDSOUNDIMPL_H_

#include <memory>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include "../DecoderImpl.h"
#include "DecoderImplDecorator.h"

namespace jukebox {

class FadeImpl: public DecoderImplDecorator {
public:
	FadeImpl(DecoderImpl *, int, int);
	virtual ~FadeImpl() = default;
	int getSamples(char *buf, int pos, int len) override;
private:
	int fadeInSecs, fadeOutSecs;
	int fadeInEndPos, fadeOutStartPos;
	std::function<void(FadeImpl&, void *, int, int)> fadeIn;
	std::function<void(FadeImpl&, void *, int, int)> fadeOut;
	static std::unordered_map<short, decltype(fadeIn)> fadeInFunc;
	static std::unordered_map<short, decltype(fadeOut)> fadeOutFunc;

	template<typename T>
	static void _fadeIn(FadeImpl &self, void* buf, int pos, int len);

	template<typename T>
	static void _fadeOut(FadeImpl &self, void* buf, int pos, int len);
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_DECORATORS_FADEDSOUNDIMPL_H_ */
