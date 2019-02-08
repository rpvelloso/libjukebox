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

#ifndef JUKEBOX_SOUND_SOUNDBUILDER_H_
#define JUKEBOX_SOUND_SOUNDBUILDER_H_

#include "jukebox/Sound/Sound.h"

namespace jukebox {

class SoundBuilder {
public:
	SoundBuilder(Sound &sound);
	SoundBuilder &reverb(float delay, float decay, size_t numDelays);
	SoundBuilder &distortion(float gain);
	SoundBuilder &fade(int fadeInSecs, int fadeOutSecs);
	SoundBuilder &resolution(int bitsPerSample);
	SoundBuilder &fadeOnStop(int fadeOutSecs);
private:
	Sound &sound;
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_SOUNDBUILDER_H_ */
