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

WaveFile::WaveFile(const std::string& filename) :
	SoundFileImpl(),
	filename(filename) {

	std::fstream file(filename, std::ios::binary|std::ios::in);
	load(file);
}

WaveFile::WaveFile(std::istream &inp) :
	SoundFileImpl(),
	filename(":stream:") {

	load(inp);
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

void WaveFile::load(std::istream &inp) {
	inp.read((char *)&header, sizeof(header));

	if (inp && inp.gcount() == sizeof(header) &&
		(std::string(header.ChunkID, 4) == "RIFF") &&
		(std::string(header.Format, 4) == "WAVE") &&
		(std::string(header.Subchunk1ID, 4) == "fmt ") &&
		(std::string(header.Subchunk2ID, 4) == "data")) {

		data.reset(new char[getDataSize()]);
		inp.read(data.get(), getDataSize());
		if (inp.gcount() != getDataSize())
			throw std::runtime_error("error loading " + filename);
	}
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

