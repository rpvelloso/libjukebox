/*
 * SoundImpl.h
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#ifndef SOUNDIMPL_H_
#define SOUNDIMPL_H_

#include "FileFormats/SoundFile.h"

namespace jukebox {

class SoundImpl {
public:
	SoundImpl(SoundFile &file);
	virtual ~SoundImpl();
	virtual void play() = 0;
	virtual void stop() = 0;
	virtual int getVolume() = 0;
	virtual void setVolume(int) = 0;
	SoundFile &getSoundFile();
protected:
	SoundFile &soundFile;
};

} /* namespace jukebox */

#endif /* SOUNDIMPL_H_ */
