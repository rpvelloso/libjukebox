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

#ifndef LINUX_ALSAHANDLE_H_
#define LINUX_ALSAHANDLE_H_

#include <thread>
#include <memory>
#include <atomic>
#include <alsa/asoundlib.h>

#include "Sound/Sound.h"
#include "Sound/SoundImpl.h"
#include "FileFormats/SoundFile.h"

namespace jukebox {

void closeAlsaHandle(snd_pcm_t *);

class AlsaHandle: public SoundImpl {
public:
	AlsaHandle(SoundFile &file);
	void play() override;
	void stop() override;
	int getVolume() override;
	void setVolume(int) override;
	~AlsaHandle();
private:
	std::unique_ptr<snd_pcm_t, decltype(&closeAlsaHandle)> handlePtr;
	std::thread playThread;
	std::atomic<bool> playing;
	int vol = 100;

	template <typename T>
	void applyVolume(T *buf, size_t len);

	void config();
	void prepare();
};

} /* namespace jukebox */

#endif /* LINUX_ALSAHANDLE_H_ */
