/*
 * FileWriterSoundImpl.cpp
 *
 *  Created on: 5 de fev de 2019
 *      Author: rvelloso
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
