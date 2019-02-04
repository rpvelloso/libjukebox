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

#include <algorithm>
#include <exception>
#include <iostream>

#include "SoundFile.h"
#include "WaveFileImpl.h"
#include "../Decoders/WavePCMDecoderImpl.h"
#include "../Decoders/WaveFloatDecoderImpl.h"

namespace jukebox {

constexpr int MAX_WAVE_SIZE = 15'000'000;

WaveFileImpl::WaveFileImpl(const std::string& filename) :
	fileStream(filename,std::ios::binary|std::ios::in),
	inputStream(fileStream.rdbuf()),
	filename(filename) {

	load();
}

WaveFileImpl::WaveFileImpl(std::istream &inp, const std::string &filename) :
	inputStream(inp.rdbuf()),
	filename(filename) {

	load();
}

short WaveFileImpl::getNumChannels() const {
	return waveFormatHeader.numChannels;
}

int WaveFileImpl::getSampleRate() const {
	return waveFormatHeader.sampleRate;
}

short WaveFileImpl::getBitsPerSample() const {
	return waveFormatHeader.bitsPerSample;
}

std::unique_ptr<Decoder> WaveFileImpl::makeDecoder() {
	switch (waveFormatHeader.audioFormat) {
	case 1:
		return std::make_unique<Decoder>(new WavePCMDecoderImpl(*this));
	case 3:
		return std::make_unique<Decoder>(new WaveFloatDecoderImpl(*this));
	default:
		throw std::runtime_error(
			"invalid audio format: " +
			std::to_string(waveFormatHeader.audioFormat) +
			". can not decode.");
	}
}

void WaveFileImpl::load() {
	// TODO use different classes of exceptions
	if (!inputStream) throw std::runtime_error("error opening " + filename);

	std::streamsize headerBufSize =
		sizeof(WaveRIFFWAVEfmtHeader) +
		sizeof(WaveFormatHeader) +
		sizeof(WaveFactHeader) +
		sizeof(WavePEAKHeader) +
		sizeof(WaveDataHeader);

	std::unique_ptr<char []> headerBuf(new char[headerBufSize]);

	inputStream.read(headerBuf.get(), headerBufSize);

	if (inputStream.gcount() != headerBufSize)
		throw std::runtime_error("error loading " + filename + ". invalid header #1 size");

	char *nextHdrPtr = headerBuf.get();
	std::copy(
		nextHdrPtr,
		nextHdrPtr + sizeof(WaveRIFFWAVEfmtHeader),
		(char *)&waveRIFFWAVEfmtHeader);

	nextHdrPtr += sizeof(WaveRIFFWAVEfmtHeader);

	std::copy(
		nextHdrPtr,
		nextHdrPtr + sizeof(WaveFormatHeader),
		(char *)&waveFormatHeader);

	nextHdrPtr += sizeof(WaveFormatHeader);

	if ((std::string(waveRIFFWAVEfmtHeader.chunkID, 4) != "RIFF") ||
		(std::string(waveRIFFWAVEfmtHeader.format, 4) != "WAVE") ||
		(std::string(waveRIFFWAVEfmtHeader.subChunkID, 4) != "fmt "))
		throw std::runtime_error("error loading " + filename + ". bad RIFF header");

	if (waveFormatHeader.audioFormat != 1 && waveFormatHeader.audioFormat != 3) // supports PCM & IEEE_FLOAT
		throw std::runtime_error("error loading " + filename + ". invalid audio format");

	nextHdrPtr += std::max(waveRIFFWAVEfmtHeader.subChunkSize - 16, 0U);

	std::string nextHeaderMarker(nextHdrPtr, nextHdrPtr + 4);
	if (nextHeaderMarker == "fact") {
		std::copy(nextHdrPtr, nextHdrPtr + sizeof(WaveFactHeader), (char *)&waveFactHeader);
		nextHdrPtr += sizeof(WaveFactHeader);
		nextHeaderMarker.assign(nextHdrPtr, nextHdrPtr + 4);
		if (nextHeaderMarker == "PEAK") {
			std::copy(nextHdrPtr, nextHdrPtr + sizeof(WavePEAKHeader), (char *)&wavePEAKHeader);
			nextHdrPtr += sizeof(WavePEAKHeader);
			nextHeaderMarker.assign(nextHdrPtr, nextHdrPtr + 4);
		}
	}

	if (nextHeaderMarker == "data")
		std::copy(nextHdrPtr, nextHdrPtr + sizeof(WaveDataHeader), (char *)&waveDataHeader);
	else
		throw std::runtime_error("error loading " + filename + ". bad data header");

	if (waveDataHeader.chunkSize <= 0)
		throw std::runtime_error("error loading " + filename + ". invalid data size");
	if (waveDataHeader.chunkSize > MAX_WAVE_SIZE)
		throw std::runtime_error("error loading " + filename + ". data size is too big");

	dataSize = waveDataHeader.chunkSize;
	inputStream.seekg(-dataSize, std::ios::end);
}

uint16_t WaveFileImpl::getAudioFormat() const {
	return waveFormatHeader.audioFormat;
}

const std::string &WaveFileImpl::getFilename() const {
	return filename;
}

int WaveFileImpl::read(char *buf, int pos, int len) {
	std::lock_guard<std::mutex> lock(readMutex);

	inputStream.clear();
	if (pos < getDataSize() && pos >= 0 && len > 0 && buf != nullptr) {
		inputStream.seekg(-(dataSize-pos), std::ios::end);
		inputStream.read(buf, len);
		return inputStream.gcount();
	}

	return -1;
}

}
