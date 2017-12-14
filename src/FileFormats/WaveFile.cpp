/*
 * WaveFile.cpp
 *
 *  Created on: 11 de dez de 2017
 *      Author: rvelloso
 */

#include <fstream>
#include <iostream>
#include <exception>

#include "WaveFile.h"

namespace jukebox {

WaveFile::WaveFile(const std::string& filename) : SoundFileImpl(filename) {
	std::fstream file(filename, std::ios::binary|std::ios::in);
	load(file);
}

WaveFile::WaveFile(std::istream &inp) : SoundFileImpl(":stream:") {
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

}
