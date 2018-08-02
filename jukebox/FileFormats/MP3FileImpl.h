/*
 * MP3FileImpl.h
 *
 *  Created on: 30 de jul de 2018
 *      Author: rvelloso
 */

#ifndef JUKEBOX_FILEFORMATS_MP3FILEIMPL_H_
#define JUKEBOX_FILEFORMATS_MP3FILEIMPL_H_

#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include "SoundFileImpl.h"
#include "jukebox/Decoders/Decoder.h"

namespace jukebox {

class MP3FileImpl : public SoundFileImpl {
public:
	MP3FileImpl(const std::string &filename);
	MP3FileImpl(std::istream& inp);
	virtual ~MP3FileImpl();
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	int getDataSize() const override;
	const std::string &getFilename() const override;
	std::unique_ptr<Decoder> makeDecoder() override;
	uint8_t *getFileBuffer();
	int getFileSize() const;
	std::vector<std::pair<long, long>> &getIndex();
private:
	std::string filename;
	int sampleRate;
	short numChannels;
	std::unique_ptr<uint8_t> fileBuffer;
	int fileSize = 0;
	int dataSize = 0;

	std::vector<std::pair<long, long> > frameIndex; // <endPos, FramePos>
	void load(std::istream& inp);
};

} /* namespace jukebox */

#endif /* JUKEBOX_FILEFORMATS_MP3FILEIMPL_H_ */
