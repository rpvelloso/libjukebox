/*
 * DirectSoundPaused.h
 *
 *  Created on: 9 de set de 2019
 *      Author: rvelloso
 */

#ifndef WIN_SOUND_STATES_DIRECTSOUNDPAUSED_H_
#define WIN_SOUND_STATES_DIRECTSOUNDPAUSED_H_

#include "DirectSoundState.h"

namespace jukebox {

class DirectSoundBuffer;

class DirectSoundPaused: public DirectSoundState {
public:
	DirectSoundPaused(DirectSoundBuffer &dsound);
	virtual ~DirectSoundPaused() = default;
	void play() override;
	void pause() override;
	int getVolume() const override;
	void setVolume(int) override;
	bool playing() const override;
	DWORD status() const override;
};

} /* namespace jukebox */

#endif /* WIN_SOUND_STATES_DIRECTSOUNDPAUSED_H_ */
