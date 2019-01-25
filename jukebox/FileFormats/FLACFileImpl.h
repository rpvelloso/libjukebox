/*
 * FLACFileImpl.h
 *
 *  Created on: 25 de jan de 2019
 *      Author: rvelloso
 */

#ifndef JUKEBOX_FILEFORMATS_FLACFILEIMPL_H_
#define JUKEBOX_FILEFORMATS_FLACFILEIMPL_H_

#include <memory>
#include "jukebox/Decoders/Decoder.h"
#include "SoundFileImpl.h"
#include "jukebox/Decoders/dr_flac/dr_flac.h"

namespace jukebox {

extern void closeFlac(drflac *);

class FLACFileImpl : public SoundFileImpl {
public:
	FLACFileImpl(const std::string &filename);
	FLACFileImpl(std::istream &inp);
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	const std::string &getFilename() const override;
	std::unique_ptr<Decoder> makeDecoder() override;
	uint8_t *getFileBuffer();
	int getFileSize() const;
private:
	short numChannels = 0;
	int sampleRate = 0;
	int fileSize = 0;
	std::unique_ptr<uint8_t []> fileBuffer;
	std::string filename;

	void load(std::istream &inp);
};

} /* namespace jukebox */

#endif /* JUKEBOX_FILEFORMATS_FLACFILEIMPL_H_ */
