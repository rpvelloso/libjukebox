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

#ifndef LIBJUKEBOX_SOUNDIMPL_2017_12_17_H_
#define LIBJUKEBOX_SOUNDIMPL_2017_12_17_H_

#include <memory>
#include <functional>
#include <map>
#include <mutex>
#include "jukebox/Decoders/Decoder.h"

namespace jukebox {

class SoundImpl {
public:
	SoundImpl(Decoder *);
	virtual ~SoundImpl() = default;
	virtual void play() = 0;
	virtual void stop() = 0;
	virtual int getVolume() const = 0;
	virtual void setVolume(int) = 0;
	virtual void loop(bool) = 0;
	virtual int getPosition() const;
	virtual void setPosition(int pos);
	virtual void setOnStopCallback(std::function<void(void)>);
	virtual void addTimedEventCallback(size_t seconds, std::function<void(void)>);
	virtual Decoder &getDecoder();
	virtual size_t getFrameSize() const;
protected:
	int position = 0;
	std::unique_ptr<Decoder> decoder;
	size_t frameSize = 0;
	std::function<void (void)> onStop;
	std::recursive_mutex timedEventsMutex;
	std::map<size_t, std::function<void (void)> > timedEvents;

	void processTimedEvents();
};

} /* namespace jukebox */

#endif
