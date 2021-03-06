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

#ifndef JUKEBOX_SOUND_DECORATORS_FADEONSTOPSOUNDIMPL_H_
#define JUKEBOX_SOUND_DECORATORS_FADEONSTOPSOUNDIMPL_H_

#include <memory>
#include "../SoundImpl.h"

namespace jukebox {

class FadeOnStopSoundImpl: public SoundImpl {
public:
	FadeOnStopSoundImpl(SoundImpl *impl, int);
	virtual ~FadeOnStopSoundImpl() = default;
	void play() override;
	void pause() override;
	void stop() override;
	int getVolume() const override;
	void setVolume(int) override;
	void loop(bool) override;
	bool playing() const override;
	int getPosition() const override;
	void setPosition(int pos) override;
	void pushOnStopCallback(std::function<void(void)> os) override;
	std::function<void(void)> popOnStopCallback() override;
	void clearOnStopStack() override;
	void addTimedEventCallback(size_t seconds, std::function<void(void)>) override;
	Decoder &getDecoder() override;
private:
	std::unique_ptr<SoundImpl> impl;
	int fadeOutSecs;
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_DECORATORS_FADEONSTOPSOUNDIMPL_H_ */
