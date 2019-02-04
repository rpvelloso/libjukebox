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

#include "FadeImpl.h"

namespace jukebox {

std::unordered_map<short, decltype(FadeImpl::fadeIn)> FadeImpl::fadeInFunc = {
		{ 8, &FadeImpl::_fadeIn<uint8_t>},
		{16, &FadeImpl::_fadeIn<int16_t>},
		{32, &FadeImpl::_fadeIn<int32_t>},
};
std::unordered_map<short, decltype(FadeImpl::fadeOut)> FadeImpl::fadeOutFunc = {
		{ 8, &FadeImpl::_fadeOut<uint8_t>},
		{16, &FadeImpl::_fadeOut<int16_t>},
		{32, &FadeImpl::_fadeOut<int32_t>},
};

FadeImpl::FadeImpl(
		DecoderImpl *impl,
		int fadeInSecs,
		int fadeOutSecs) :
	DecoderImpl(impl->getFileImpl()),
	impl(impl),
	fadeInSecs(fadeInSecs),
	fadeOutSecs(fadeOutSecs),
	fadeInEndPos(0),
	fadeOutStartPos(getDataSize()) {

	if (fadeInSecs > 0)
		fadeInEndPos = std::min(
				getSampleRate()*
				getNumChannels()*
				(getBitsPerSample() >> 3)*
				fadeInSecs, getDataSize());

	if (fadeOutSecs > 0)
		fadeOutStartPos -= std::min(
				getSampleRate()*
				getNumChannels()*
				(getBitsPerSample() >> 3)*
				fadeOutSecs, getDataSize());

	fadeIn = fadeInFunc[getBitsPerSample()];
	fadeOut = fadeOutFunc[getBitsPerSample()];
}

int jukebox::FadeImpl::getSamples(char* buf, int pos, int len) {
	auto ret = impl->getSamples(buf, pos, len);

	auto dataSize = getDataSize();

	if (pos + len > dataSize)
		len = (dataSize - pos);

	if (pos < fadeInEndPos)
		fadeIn(*this, buf, pos, len);

	if (pos >= fadeOutStartPos)
		fadeOut(*this, buf, pos, len);

	return ret;
}

} /* namespace jukebox */

