/*
    Copyright 2019 Roberto Panerai Velloso.
    This file is part of libjukebox.
    libjukebox is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libjukebox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libjukebox.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WIN_SOUND_STATES_DIRECTSOUNDPLAYING_H_
#define WIN_SOUND_STATES_DIRECTSOUNDPLAYING_H_

#include <thread>
#include <atomic>
#include <memory>
#include <dsound.h>
#include <mmsystem.h>

#include "DirectSoundState.h"

namespace jukebox {

extern void ReleaseBuffer(LPDIRECTSOUNDBUFFER);

enum class PlayingStatus : int {
	STOPPED = 0,
	PAUSED = 1,
	PLAYING = 2
};

class DirectSoundPlaying: public DirectSoundState {
public:
	DirectSoundPlaying(DirectSoundBuffer &dsound);
	virtual ~DirectSoundPlaying() = default;
	void play() override;
	void pause() override;
	void stop() override;
	int getVolume() const override;
	void setVolume(int) override;
	bool playing() const override;
	DWORD status() const override;
private:
	WAVEFORMATEX wfx;
	DSBUFFERDESC dsbdesc;
	std::unique_ptr<struct IDirectSoundBuffer, decltype(&ReleaseBuffer)> pDsb;
	std::thread loadBufferThread;
	std::atomic<PlayingStatus> playingStatus;

	bool fillBuffer(int offset, size_t size);
	DWORD startThread();
};

} /* namespace jukebox */

#endif /* WIN_SOUND_STATES_DIRECTSOUNDPLAYING_H_ */
