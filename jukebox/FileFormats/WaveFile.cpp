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
#include <exception>
#include <string>

#include "WaveFile.h"
#include "SoundFile.h"

namespace jukebox {

constexpr int MAX_WAVE_SIZE = 15'000'000;

WaveFile::WaveFile(const std::string& filename) :
	WaveFile(std::fstream(filename,std::ios::binary|std::ios::in),filename) {
}

WaveFile::WaveFile(std::istream &&inp, const std::string &filename) :
	WaveFile(inp,filename) {
}

WaveFile::WaveFile(std::istream &inp, const std::string &filename) :
	filename(filename) {
	// TODO use different classes of exceptions
	if (!inp) throw std::runtime_error("error opening " + filename);

	inp.read((char *)&header, sizeof(header));
	if (inp.gcount() != sizeof(header)) throw std::runtime_error("error loading " + filename + ". invalid header size");

	if ((std::string(header.ChunkID, 4) != "RIFF") ||
		(std::string(header.Format, 4) != "WAVE") ||
		(std::string(header.Subchunk1ID, 4) != "fmt ") ||
		(std::string(header.Subchunk2ID, 4) != "data")) {
		throw std::runtime_error("error loading " + filename + ". bad header data");
	}

	if (header.Subchunk2Size <= 0) throw std::runtime_error("error loading " + filename + ". invalid data size");
	if (header.Subchunk2Size > MAX_WAVE_SIZE) throw std::runtime_error("error loading " + filename + ". data size is too big");

	data.reset(new char[getDataSize()]);
	inp.read(data.get(), getDataSize());
	if (inp.gcount() != getDataSize()) throw std::runtime_error("error loading " + filename + ". not enough data to load");
}

short WaveFile::getNumChannels() const {
	return header.NumChannels;
}

int WaveFile::getSampleRate() const {
	return header.SampleRate;
}

short WaveFile::getBitsPerSample() const {
	return header.BitsPerSample;
}

int WaveFile::getDataSize() const {
	return header.Subchunk2Size;
}

const char* WaveFile::getData() const {
	return data.get();
}

const std::string &WaveFile::getFilename() const {
	return filename;
}

namespace factory {
	SoundFile loadWaveFile(const std::string &filename) {
		return SoundFile(new WaveFile(filename));
	}

	SoundFile loadWaveStream(std::istream &inp) {
		return SoundFile(new WaveFile(inp));
	}
}

}

