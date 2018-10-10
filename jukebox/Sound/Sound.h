/*
    Copyright 2017 Roberto Panerai Velloso.
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

#ifndef LIBJUKEBOX_SOUND_2017_12_17_H_
#define LIBJUKEBOX_SOUND_2017_12_17_H_

#include <memory>

#include "SoundImpl.h"

namespace jukebox {

class Sound {
public:
	Sound(SoundImpl *impl);
	void play();
	void stop();
	int getVolume() const;
	void setVolume(int);
	void loop(bool);
	int getPosition() const;
	/*
	 * this onStop event is need, so the client can be notified
	 * when the sound stops and does not need to busy wait (poll) for it.
	 * */
	void setOnStopCallback(std::function<void(void)>);
	void clearOnStopCallback();
private:
	std::unique_ptr<SoundImpl> impl;
};

} /* namespace jukebox */

#endif
