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

#ifndef WIN_SOUND_STATES_DIRECTSOUNDPAUSED_H_
#define WIN_SOUND_STATES_DIRECTSOUNDPAUSED_H_

#include "DirectSoundState.h"

namespace jukebox {

class DirectSoundBuffer;

class DirectSoundPaused: public DirectSoundState {
public:
	DirectSoundPaused(DirectSoundState &state);
	virtual ~DirectSoundPaused() = default;
	void play() override;
	void restart() override;
	void pause() override;
	void stop() override;
	int getVolume() const override;
	void setVolume(int) override;
	bool playing() const override;
	DWORD status() const override;
};

} /* namespace jukebox */

#endif /* WIN_SOUND_STATES_DIRECTSOUNDPAUSED_H_ */
