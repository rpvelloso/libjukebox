/*
 * DirectSoundState.h
 *
 *  Created on: 9 de set de 2019
 *      Author: rvelloso
 */

#ifndef WIN_SOUND_STATES_DIRECTSOUNDSTATE_H_
#define WIN_SOUND_STATES_DIRECTSOUNDSTATE_H_

#include <windows.h>

#include "../DirectSoundBuffer.h"

namespace jukebox {

class DirectSoundState {
public:
	DirectSoundState(DirectSoundBuffer &dsound);
	virtual ~DirectSoundState() = default;
	virtual void play() = 0;
	virtual void pause() = 0;
	virtual int getVolume() const = 0;
	virtual void setVolume(int) = 0;
	virtual bool playing() const = 0;
	virtual DWORD status() const = 0;
protected:
	DirectSoundBuffer &dsound;
};

} /* namespace jukebox */

#endif /* WIN_SOUND_STATES_DIRECTSOUNDSTATE_H_ */
