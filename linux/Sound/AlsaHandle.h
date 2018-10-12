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

#ifndef LIBJUKEBOX_LINUX_ALSAHANDLE_2017_12_28_H_
#define LIBJUKEBOX_LINUX_ALSAHANDLE_2017_12_28_H_

#include <atomic>
#include <memory>
#include <thread>
#include <alsa/asoundlib.h>

#include "jukebox/FileFormats/SoundFile.h"
#include "jukebox/Sound/SoundImpl.h"
#include "jukebox/Decoders/Decoder.h"

namespace jukebox {

extern void closeAlsaHandle(snd_pcm_t *);

class AlsaHandle: public SoundImpl {
public:
	AlsaHandle(SoundFile &file);
	void play() override;
	void stop() override;
	int getVolume() const override;
	void setVolume(int) override;
	void loop(bool) override;
	~AlsaHandle();
private:
	std::unique_ptr<snd_pcm_t, decltype(&closeAlsaHandle)> handlePtr;
	std::thread playThread;
	std::atomic<bool> playing;
	int vol = 100;
	bool looping = false;
	std::unique_ptr<Decoder> decoder;

	template <typename T>
	void applyVolume(T *buf, int position, int len);

	void config();
	void prepare();
};

} /* namespace jukebox */

#endif
