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

#include <cmath>
#include <exception>

#include "SoundFile.h"

namespace jukebox {

SoundFile::SoundFile(SoundFileImpl *impl) :
		impl(impl),
		blockSize(impl->getNumChannels() * impl->getBitsPerSample()/8) {
};

short SoundFile::getNumChannels() const {
	return impl->getNumChannels();
};

int SoundFile::getSampleRate() const {
	return impl->getSampleRate();
};

short SoundFile::getBitsPerSample() const {
	return impl->getBitsPerSample();
};

int SoundFile::getDataSize() const {
	return impl->getDataSize();
};

// sound duration in seconds
double SoundFile::getDuration() const {
	double rate = impl->getSampleRate();
	double chan = impl->getNumChannels();
	double res = impl->getBitsPerSample()/8;
	double len = impl->getDataSize();
	return std::round(len/(rate*chan*res));
}

const std::string& SoundFile::getFilename() const {
	return impl->getFilename();
}

void jukebox::SoundFile::truncAt(int pos) {
	impl->truncAt(pos);
}

std::unique_ptr<Decoder> SoundFile::makeDecoder() {
	return impl->makeDecoder();
}

} /* namespace jukebox */

