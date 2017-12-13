/*
 * DirectSoundBuffer.h
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#ifndef DIRECTSOUNDBUFFER_H_
#define DIRECTSOUNDBUFFER_H_

#include <windows.h>
#include <dsound.h>

#include "SoundImpl.h"
#include "Sound.h"
#include "FileFormats/SoundFile.h"

namespace jukebox {

void ReleaseBuffer(LPDIRECTSOUNDBUFFER);

class DirectSoundBuffer: public SoundImpl {
public:
	DirectSoundBuffer(SoundFile &file);
	void play() override;
	void stop() override;
	int getVolume() override;
	void setVolume(int) override;
private:
	SoundFile &soundFile;
	WAVEFORMATEX wfx;
	DSBUFFERDESC dsbdesc;
	std::unique_ptr<struct IDirectSoundBuffer, decltype(&ReleaseBuffer)> pDsb;

	void prepare();
};

namespace factory {
	Sound makeSound(SoundFile &file);
}

} /* namespace jukebox */

#endif /* DIRECTSOUNDBUFFER_H_ */
