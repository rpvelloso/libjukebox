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
