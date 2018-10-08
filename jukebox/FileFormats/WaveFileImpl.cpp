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

#include "jukebox/Decoders/WaveDecoderImpl.h"
#include "SoundFile.h"
#include "WaveFileImpl.h"

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
	return header2.NumChannels;
}

int WaveFileImpl::getSampleRate() const {
	return header2.SampleRate;
}

short WaveFileImpl::getBitsPerSample() const {
	return header2.BitsPerSample;
}

std::unique_ptr<Decoder> WaveFileImpl::makeDecoder() {
	return std::make_unique<Decoder>(new WaveDecoderImpl(*this));
}

void WaveFileImpl::load() {
	// TODO use different classes of exceptions
	if (!inputStream) throw std::runtime_error("error opening " + filename);

	inputStream.read((char *)&header1, sizeof(header1));
	if (inputStream.gcount() != sizeof(header1))
		throw std::runtime_error("error loading " + filename + ". invalid header #1 size");

	if ((std::string(header1.ChunkID, 4) != "RIFF") ||
		(std::string(header1.Format, 4) != "WAVE") ||
		(std::string(header1.Subchunk1ID, 4) != "fmt "))
		throw std::runtime_error("error loading " + filename + ". bad header #1 data");

	inputStream.read((char *)&header2, sizeof(header2));
	if (inputStream.gcount() != sizeof(header2))
		throw std::runtime_error("error loading " + filename + ". invalid header #2 size");

	if (header2.AudioFormat != 1) // PCM
		throw std::runtime_error("error loading " + filename + ". invalid audio format");

	// skip extra header data
	inputStream.seekg(
		std::max(header1.Subchunk1Size - 16, 0U),
		std::ios::cur);

	inputStream.read((char *)&header3, sizeof(header3));
	if (inputStream.gcount() != sizeof(header3))
		throw std::runtime_error("error loading " + filename + ". invalid header #3 size");

	if (std::string(header3.Subchunk2ID, 4) != "data")
		throw std::runtime_error("error loading " + filename + ". bad header #3 data");

	if (header3.Subchunk2Size <= 0)
		throw std::runtime_error("error loading " + filename + ". invalid data size");
	if (header3.Subchunk2Size > MAX_WAVE_SIZE)
		throw std::runtime_error("error loading " + filename + ". data size is too big");

	dataSize = header3.Subchunk2Size;
	headerSize = inputStream.tellg();
}

const std::string &WaveFileImpl::getFilename() const {
	return filename;
}

int WaveFileImpl::read(char *buf, int pos, int len) {
	std::lock_guard<std::mutex> lock(readMutex);

	inputStream.clear();
	if (pos < getDataSize() && pos >= 0 && len > 0 && buf != nullptr) {
		inputStream.seekg(headerSize + pos, std::ios::beg);
		inputStream.read(buf, len);
		return inputStream.gcount();
	}

	return -1;
}

}
