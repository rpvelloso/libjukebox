/*
 * SoundFile.h
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#ifndef SOUNDFILE_H_
#define SOUNDFILE_H_

#include <memory>

#include "SoundFileImpl.h"

namespace jukebox {

class SoundFile {
public:
	SoundFile(SoundFileImpl *impl);
	short getNumChannels() const;
	int getSampleRate() const;
	short getBitsPerSample() const;
	const char *getData() const;
	int getDataSize() const;
	const std::string &getFilename() const;
	double getDuration() const;
private:
	std::unique_ptr<SoundFileImpl> impl;
};

} /* namespace jukebox */

#endif /* SOUNDFILE_H_ */
