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

#ifndef LIBJUKEBOX_LINUX_ALSAMIXER_2017_12_28_H_
#define LIBJUKEBOX_LINUX_ALSAMIXER_2017_12_28_H_

#include <memory>
#include <alsa/asoundlib.h>

#include "jukebox/Mixer/MixerImpl.h"

namespace jukebox {

extern void closeMixer(snd_mixer_t *handle);

class AlsaMixer : public MixerImpl {
	//friend MixerImpl &factory::makeMixerImpl();
public:
	AlsaMixer(const std::string &devName, const std::string &element);
	int getVolume();
	void setVolume(int vol);
private:
	std::unique_ptr<snd_mixer_t, decltype(&closeMixer)> handlePtr;
	snd_mixer_elem_t* element_handle = nullptr;
	long minVolume, maxVolume;
};

} /* namespace jukebox */

#endif
