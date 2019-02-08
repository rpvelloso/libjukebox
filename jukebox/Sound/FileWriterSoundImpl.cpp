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

#include <memory>
#include <fstream>

#include "FileWriterSoundImpl.h"

namespace jukebox {

struct WaveHeader {
	char RIFF[4] = {'R','I','F', 'F'};
	uint32_t chunkSize;
	char WAVE[4] = {'W', 'A', 'V', 'E'};
	char fmt[4] = {'f', 'm', 't', ' '};
	uint32_t subChunkSize = 0x10;

	uint16_t audioFormat = 1;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;

	char data[4] = {'d', 'a', 't', 'a'};
	uint32_t dataSize;
};

FileWriterSoundImpl::FileWriterSoundImpl(Decoder *decoder, std::string filename) :
	SoundImpl(decoder),
	filename(filename) {
}

void FileWriterSoundImpl::play() {
	std::fstream output(filename, std::ios::binary|std::ios::trunc|std::ios::out);
	WaveHeader waveHeader;

	waveHeader.dataSize = decoder->getDataSize();
	waveHeader.chunkSize = waveHeader.dataSize + 36;
	waveHeader.numChannels = decoder->getNumChannels();
	waveHeader.sampleRate = decoder->getSampleRate();
	waveHeader.bitsPerSample = decoder->getBitsPerSample();
	waveHeader.byteRate = waveHeader.sampleRate * waveHeader.numChannels * (waveHeader.bitsPerSample >> 3);
	waveHeader.blockAlign = waveHeader.numChannels * (waveHeader.bitsPerSample >> 3);

	output.write((char *)&waveHeader, sizeof(WaveHeader));

	int bufSize = waveHeader.blockAlign * 4096;
	int pos = 0;
	std::unique_ptr<char []> buf(new char[bufSize]);

	auto len = decoder->getSamples(buf.get(), pos, bufSize);
	while (len > 0) {
		pos += len;
		output.write(buf.get(), len);
		len = decoder->getSamples(buf.get(), pos, bufSize);
	}
	onStop();
}

void FileWriterSoundImpl::stop() {
}

int FileWriterSoundImpl::getVolume() const {
	return 100;
}

void FileWriterSoundImpl::setVolume(int vol) {
}

void FileWriterSoundImpl::loop(bool l) {
}

} /* namespace jukebox */
