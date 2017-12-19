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

#include <fstream>
#include <iostream>
#include <exception>

#include "FileFormats/WaveFile.h"

namespace jukebox {

constexpr int MAX_WAVE_SIZE = 15'000'000;

WaveFile::WaveFile(const std::string& filename) :
	SoundFileImpl(),
	filename(filename) {

	std::fstream file(this->filename, std::ios::binary|std::ios::in);
	load(file);
}

WaveFile::WaveFile(std::istream &inp) :
	SoundFileImpl(),
	filename(":stream:") {

	load(inp);
}

short WaveFile::getNumChannels() const {
	return header2.NumChannels;
}

int WaveFile::getSampleRate() const {
	return header2.SampleRate;
}

short WaveFile::getBitsPerSample() const {
	return header2.BitsPerSample;
}

int WaveFile::getDataSize() const {
	return header3.Subchunk2Size;
}

void WaveFile::load(std::istream &inp) {
	if (!inp) throw std::runtime_error("error opening " + filename);

	inp.read((char *)&header1, sizeof(header1));
	if (inp.gcount() != sizeof(header1))
		throw std::runtime_error("error loading " + filename + ". invalid header #1 size");

	if ((std::string(header1.ChunkID, 4) != "RIFF") ||
		(std::string(header1.Format, 4) != "WAVE") ||
		(std::string(header1.Subchunk1ID, 4) != "fmt "))
		throw std::runtime_error("error loading " + filename + ". bad header #1 data");

	inp.read((char *)&header2, sizeof(header2));
	if (inp.gcount() != sizeof(header2))
		throw std::runtime_error("error loading " + filename + ". invalid header #2 size");

	if (header2.AudioFormat != 1) // PCM
		throw std::runtime_error("error loading " + filename + ". invalid audio format");

	// skip extra header data
	inp.seekg(
		std::max(header1.Subchunk1Size - 16, 0),
		std::ios::cur);

	inp.read((char *)&header3, sizeof(header3));
	if (inp.gcount() != sizeof(header3))
		throw std::runtime_error("error loading " + filename + ". invalid header #3 size");

	if (std::string(header3.Subchunk2ID, 4) != "data")
		throw std::runtime_error("error loading " + filename + ". bad header #3 data");

	if (header3.Subchunk2Size <= 0)
		throw std::runtime_error("error loading " + filename + ". invalid data size");
	if (header3.Subchunk2Size > MAX_WAVE_SIZE)
		throw std::runtime_error("error loading " + filename + ". data size is too big");

	data.reset(new char[getDataSize()]);
	inp.read(data.get(), getDataSize());

	if (inp.gcount() != getDataSize())
		throw std::runtime_error("error loading " + filename + ". not enough data to load");
}

const char* WaveFile::getData() const {
	return data.get();
}

namespace factory {
	SoundFile loadWaveFile(const std::string &filename) {
		return SoundFile(new WaveFile(filename));
	}

	SoundFile loadWaveStream(std::istream &inp) {
		return SoundFile(new WaveFile(inp));
	}
}

const std::string& WaveFile::getFilename() const {
	return filename;
}

}

