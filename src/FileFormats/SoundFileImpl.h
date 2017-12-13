/*
 * SoundFileImpl.h
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#ifndef SOUNDFILEIMPL_H_
#define SOUNDFILEIMPL_H_

#include <string>

namespace jukebox {

class SoundFileImpl {
public:
	SoundFileImpl(const std::string &filename);
	virtual ~SoundFileImpl();
	virtual short getNumChannels() const = 0;
	virtual int getSampleRate() const = 0;
	virtual short getBitsPerSample() const = 0;
	virtual const char *getData() const = 0;
	virtual int getDataSize() const = 0;
	const std::string &getFilename() const;
protected:
	std::string filename;
};

} /* namespace jukebox */

#endif /* SOUNDFILEIMPL_H_ */
