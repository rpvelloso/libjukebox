/*
 * Sound.h
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#ifndef SOUND_H_
#define SOUND_H_

#include <memory>

#include "SoundImpl.h"

namespace jukebox {

class Sound {
public:
	Sound(SoundImpl *impl);
	void play();
	void stop();
	int getVolume();
	void setVolume(int);
private:
	std::unique_ptr<SoundImpl> impl;
};

} /* namespace jukebox */

#endif /* SOUND_H_ */
