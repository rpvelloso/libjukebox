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

#ifndef JUKEBOX_DECODERS_DECORATORS_JOINTSTEREOIMPL_H_
#define JUKEBOX_DECODERS_DECORATORS_JOINTSTEREOIMPL_H_

#include <unordered_map>
#include <functional>
#include "DecoderImplDecorator.h"

namespace jukebox {

class JointStereoImpl : public DecoderImplDecorator {
public:
	JointStereoImpl(DecoderImpl *impl);
	virtual ~JointStereoImpl() = default;
	int getSamples(char *buf, int pos, int len) override;
	short getNumChannels() const override;
	int getDataSize() const override;
	int getBlockSize() const override;
private:
	std::function<void(void *, void *, int)> mixChannels;
	static std::unordered_map<int, decltype(mixChannels)> mixChannelsFunc;

	template <typename T>
	static void _mixChannels(void *input, void *output, int len);
};

} /* namespace jukebox */

#endif /* JUKEBOX_DECODERS_DECORATORS_JOINTSTEREOIMPL_H_ */
