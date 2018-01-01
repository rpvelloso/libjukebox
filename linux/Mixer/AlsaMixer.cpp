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

#include <exception>
#include "AlsaMixer.h"

namespace jukebox {

void closeMixer(snd_mixer_t *handle) {
	if (handle != nullptr)
		snd_mixer_close(handle);
}

AlsaMixer::AlsaMixer(const std::string &devName, const std::string &element) :
	handlePtr(nullptr, closeMixer) {

	snd_mixer_t *handle;

	// open
	auto res = snd_mixer_open(&handle, 0);
	if (res != 0)
		throw std::runtime_error("snd_mixer_open error.");
	handlePtr.reset(handle);

	// attach
	res = snd_mixer_attach(handlePtr.get(), devName.c_str());
	if (res != 0)
		throw std::runtime_error("snd_mixer_attach error.");

	// register
	res = snd_mixer_selem_register(handlePtr.get(), nullptr, nullptr);
	if (res != 0)
		throw std::runtime_error("snd_mixer_selem_register error.");

	res = snd_mixer_load(handlePtr.get());
	if (res != 0)
		throw std::runtime_error("snd_mixer_load error.");

	snd_mixer_selem_id_t* id;

	snd_mixer_selem_id_alloca(&id);
	snd_mixer_selem_id_set_index(id, 0);
	snd_mixer_selem_id_set_name(id, element.c_str());
	element_handle = snd_mixer_find_selem(handlePtr.get(), id);

	res = snd_mixer_selem_get_playback_volume_range(element_handle, &minVolume, &maxVolume);
	if (res != 0)
		throw std::runtime_error("snd_mixer_selem_get_playback_volume_range error.");
}

int AlsaMixer::getVolume() {
	long alsaVol;

	auto res = snd_mixer_selem_get_playback_volume(element_handle, SND_MIXER_SCHN_MONO, &alsaVol);
	if (res != 0)
		throw std::runtime_error("snd_mixer_selem_get_playback_volume error.");

	return (alsaVol*100)/maxVolume;
}

void AlsaMixer::setVolume(int vol) {
	long alsaVol = (vol*maxVolume)/100;

	auto res = snd_mixer_selem_set_playback_volume_all(element_handle, alsaVol);
	if (res != 0)
		throw std::runtime_error("snd_mixer_selem_set_playback_volume_all error.");
}

namespace factory {
	MixerImpl &makeMixerImpl() {
		static AlsaMixer mixerImpl("default", "Master");
		return mixerImpl;
	}
}

} /* namespace jukebox */
