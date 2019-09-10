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

#ifndef LINUX_SOUND_STATES_ALSAPLAYING_H_
#define LINUX_SOUND_STATES_ALSAPLAYING_H_

#include <thread>
#include <functional>
#include <unordered_map>
#include <memory>
#include <atomic>

#include <alsa/asoundlib.h>

#include "AlsaState.h"

namespace jukebox {

extern void closeAlsaHandle(snd_pcm_t *);

class AlsaPlaying: public AlsaState {
public:
	AlsaPlaying(AlsaHandle &alsa);
	virtual ~AlsaPlaying();
	void play() override;
	void pause() override;
	int getVolume() const override;
	void setVolume(int) override;
	bool playing() const override;
private:
	std::unique_ptr<snd_pcm_t, decltype(&closeAlsaHandle)> handlePtr;
	std::thread playThread;
	std::atomic<bool> isPlaying;
	int vol = 100;
	snd_pcm_uframes_t bufferSize = 0;
	std::function<void(AlsaHandle &self, void *, int , int )> applyVolume;
	std::function<decltype(snd_pcm_drain)> clearBuffer = snd_pcm_drain;
	static std::unordered_map<short, decltype(applyVolume)> applyVolumeFunc;

	template <typename T>
	static void _applyVolume(AlsaHandle &self, void *buf, int position, int len);
};

} /* namespace jukebox */

#endif /* LINUX_SOUND_STATES_ALSAPLAYING_H_ */