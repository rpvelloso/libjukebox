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
		blockSize(impl->getNumChannels() * impl->getBitsPerSample()/8),
		dataSize(impl->getDataSize()) {
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
	return dataSize;
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

int SoundFile::read(char* buf, int pos, int len) {
	if (pos > dataSize)
		return 0;
	if (pos + len > dataSize)
		len = dataSize - pos;

	if (len % blockSize != 0)
		throw std::runtime_error("invalid buffer size, should be block aligned.");
	if (pos % blockSize != 0)
		throw std::runtime_error("invalid stream position, should be block aligned.");

	return impl->read(buf, pos, len);
}

void jukebox::SoundFile::truncAt(int pos) {
	dataSize = std::min(pos, dataSize);
}

std::unique_ptr<Decoder> SoundFile::makeDecoder() {
	return impl->makeDecoder();
}

} /* namespace jukebox */
