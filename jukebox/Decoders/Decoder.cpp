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
#include "Decoder.h"
#include "jukebox/FileFormats/SoundFile.h"
#include "jukebox/FileFormats/SoundFileImpl.h"

namespace jukebox {

Decoder::Decoder(SoundFile soundFile) :
		soundFileImpl(soundFile.impl),
		impl(soundFileImpl->makeDecoder()) {
}

const std::string& Decoder::getFilename() const {
	return soundFileImpl->getFilename();
}

double Decoder::getDuration() const {
	double rate = impl->getSampleRate();
	double chan = impl->getNumChannels();
	double res = impl->getBitsPerSample() >> 3;
	double len = impl->getDataSize();
	return std::round(len/(rate*chan*res));
}

Decoder::Decoder(std::shared_ptr<SoundFileImpl> soundFileImpl) :
		soundFileImpl(soundFileImpl),
		impl(soundFileImpl->makeDecoder()) {
}

Decoder *Decoder::prototype() {
	return new Decoder(soundFileImpl);
}

int Decoder::getSamples(char* buf, int pos, int len) {
	if (len % impl->getBlockSize() != 0)
		throw std::runtime_error("invalid buffer size, should be block aligned.");

	if (pos >= impl->getDataSize())
		return -1;

	if (pos + len > impl->getDataSize())
		len = impl->getDataSize() - pos;

	return impl->getSamples(buf, pos, len);
}

short Decoder::getNumChannels() const {
	return impl->getNumChannels();
}

int Decoder::getSampleRate() const {
	return impl->getSampleRate();
}

short Decoder::getBitsPerSample() const {
	return impl->getBitsPerSample();
}

int Decoder::getDataSize() const {
	return impl->getDataSize();
}

int Decoder::silenceLevel() const {
	return impl->silenceLevel();
}

Decoder &Decoder::peel() {
	auto dec = impl->peel();
	if (dec != nullptr)
		impl.reset(dec);
	return *this;
}

int Decoder::getBlockSize() const {
	return impl->getBlockSize();
}

}

