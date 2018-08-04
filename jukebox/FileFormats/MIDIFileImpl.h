/*
 * MIDIFileImpl.h
 *
 *  Created on: 3 de ago de 2018
 *      Author: rvelloso
 */

#ifndef JUKEBOX_FILEFORMATS_MIDIFILEIMPL_H_
#define JUKEBOX_FILEFORMATS_MIDIFILEIMPL_H_

#include <memory>
#include <string>
#include "SoundFileImpl.h"

namespace jukebox {

class MIDIFileImpl: public SoundFileImpl {
public:
	MIDIFileImpl(const std::string &filename);
	MIDIFileImpl(std::istream& inp);
	virtual ~MIDIFileImpl() = default;
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	int getDataSize() const override;
	const std::string &getFilename() const override;
	std::unique_ptr<Decoder> makeDecoder() override;
	uint8_t *getFileBuffer();
	int getFileSize();
private:
	std::string filename;
	int fileSize = 0;
	int dataSize = 0;
	std::unique_ptr<uint8_t> fileBuffer;

	void load(std::istream& inp);
};

} /* namespace jukebox */

#endif /* JUKEBOX_FILEFORMATS_MIDIFILEIMPL_H_ */
