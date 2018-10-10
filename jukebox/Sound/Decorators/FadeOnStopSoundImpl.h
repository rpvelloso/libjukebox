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

#ifndef JUKEBOX_SOUND_FADEONSTOPSOUNDIMPL_H_
#define JUKEBOX_SOUND_FADEONSTOPSOUNDIMPL_H_

#include "../SoundImpl.h"

namespace jukebox {

class FadeOnStopSoundImpl: public SoundImpl {
public:
	FadeOnStopSoundImpl(SoundImpl *, int);
	virtual ~FadeOnStopSoundImpl() = default;
	void play() override;
	void stop() override;
	int getVolume() const override;
	void setVolume(int) override;
	void loop(bool) override;
	void setOnStopCallback(std::function<void(void)>) override;
private:
	std::unique_ptr<SoundImpl> impl;
	int fadeOutSecs;
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_FADEONSTOPSOUNDIMPL_H_ */
