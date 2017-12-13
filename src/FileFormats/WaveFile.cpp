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
	load(this->filename);
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

void WaveFile::load(const std::string& filename) {
	std::fstream file(filename, std::ios::binary|std::ios::in);

	file.read((char *)&header, sizeof(header));

	if (file &&
		(std::string(header.ChunkID, 4) == "RIFF") &&
		(std::string(header.Format, 4) == "WAVE") &&
		(std::string(header.Subchunk1ID, 4) == "fmt ") &&
		(std::string(header.Subchunk2ID, 4) == "data")) {

		data.reset(new char[getDataSize()]);
		file.read(data.get(), getDataSize());
		if (file.gcount() != getDataSize())
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
}

}
