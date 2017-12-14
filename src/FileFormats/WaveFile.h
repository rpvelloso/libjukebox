/*
 * WaveFile.h
 *
 *  Created on: 11 de dez de 2017
 *      Author: rvelloso
 */

#ifndef WAVEFILE_H_
#define WAVEFILE_H_

#include <memory>

#include "SoundFile.h"
#include "SoundFileImpl.h"

namespace jukebox {

class WaveFile : public SoundFileImpl {
	struct WaveHeader {
		char ChunkID[4];
		int ChunkSize;
		char Format[4];
		char Subchunk1ID[4];
		int Subchunk1Size;
		short AudioFormat;
		short NumChannels;
		int SampleRate;
		int ByteRate;
		short BlockAlign;
		short BitsPerSample;
		char Subchunk2ID[4];
		int Subchunk2Size;
	};
public:
	WaveFile(const std::string &filename);
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	const char *getData() const override;
	int getDataSize() const override;
private:
	WaveHeader header;
	std::unique_ptr<char []> data;

	void load(const std::string &filename);
};

namespace factory {
	SoundFile loadWaveFile(const std::string &filename);
}

}

#endif /* WAVEFILE_H_ */
