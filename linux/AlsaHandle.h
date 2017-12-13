/*
 * AlsaHandle.h
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#ifndef LINUX_ALSAHANDLE_H_
#define LINUX_ALSAHANDLE_H_

#include <thread>
#include <memory>
#include <alsa/asoundlib.h>

#include "Sound.h"
#include "SoundImpl.h"
#include "FileFormats/SoundFile.h"

namespace jukebox {

void closeAlsaHandle(snd_pcm_t *);

class AlsaHandle: public SoundImpl {
public:
	AlsaHandle(SoundFile &file);
	void play();
	~AlsaHandle();
private:
	SoundFile &soundFile;
	std::unique_ptr<snd_pcm_t, decltype(&closeAlsaHandle)> handlePtr;
	std::thread playThread;

	void prepare();
};

namespace factory {
	Sound makeSound(SoundFile &file);
}

} /* namespace jukebox */

#endif /* LINUX_ALSAHANDLE_H_ */
