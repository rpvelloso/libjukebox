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

#ifndef LIBJUKEBOX_DIRECTSOUNDBUFFER_2017_12_17_H_
#define LIBJUKEBOX_DIRECTSOUNDBUFFER_2017_12_17_H_

#include <thread>

#include "jukebox/Sound/SoundImpl.h"
#include "jukebox/Decoders/Decoder.h"

namespace jukebox {

class DirectSoundState;

class DirectSoundBuffer: public SoundImpl {
public:
	DirectSoundBuffer(Decoder *);
	~DirectSoundBuffer();
	void play() override;
	void pause() override;
	void stop() override;
	int getVolume() const override;
	void setVolume(int) override;
	void loop(bool) override;
	bool playing() const override;
	template<class T>
	void setState() {
		state.reset(new T(*state));
	};
	bool isLooping() const;
private:
	std::unique_ptr<DirectSoundState> state;
	bool looping = false;

};

} /* namespace jukebox */

#endif
