/*
 * DirectSoundPlaying.h
 *
 *  Created on: 9 de set de 2019
 *      Author: rvelloso
 */

#ifndef WIN_SOUND_STATES_DIRECTSOUNDPLAYING_H_
#define WIN_SOUND_STATES_DIRECTSOUNDPLAYING_H_

#include <thread>
#include <memory>
#include <dsound.h>
#include <mmsystem.h>

#include "DirectSoundState.h"

namespace jukebox {

extern void ReleaseBuffer(LPDIRECTSOUNDBUFFER);

class DirectSoundPlaying: public DirectSoundState {
public:
	DirectSoundPlaying(DirectSoundBuffer &dsound);
	virtual ~DirectSoundPlaying();
	void play() override;
	void pause() override;
	int getVolume() const override;
	void setVolume(int) override;
	bool playing() const override;
	DWORD status() const override;
private:
	WAVEFORMATEX wfx;
	DSBUFFERDESC dsbdesc;
	std::unique_ptr<struct IDirectSoundBuffer, decltype(&ReleaseBuffer)> pDsb;
	std::thread loadBufferThread;

	bool fillBuffer(int offset, size_t size);
	DWORD startThread();
};

} /* namespace jukebox */

#endif /* WIN_SOUND_STATES_DIRECTSOUNDPLAYING_H_ */
